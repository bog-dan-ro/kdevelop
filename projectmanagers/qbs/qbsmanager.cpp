/* KDevelop QBS Support
 *
 * Copyright 2015 BogDan Vatra <bogdan@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "qbsimportjob.h"
#include "qbsitems.h"
#include "qbskjob.h"
#include "qbsmanager.h"
#include "qbsprojectconfig.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(QBS)
Q_LOGGING_CATEGORY(QBS, "kdevelop.projectmanagers.qbs")

#include <KDirWatch>
#include <KLocalizedString>
#include <KPluginFactory>
#include <QDirIterator>

#include <algorithm>

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iruncontroller.h>

#include <project/helper.h>
#include <project/projectmodel.h>

using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(QBSSupportFactory, "kdevqbsmanager.json", registerPlugin<QbsManager>(); )

namespace {
    const QLatin1String CPP_MODULE("cpp");
    const QLatin1String DEFINES("defines");
    const QLatin1String INCLUDEPATHS("includePaths");
    const QLatin1String SYSTEM_INCLUDEPATHS("systemIncludePaths");

    const QLatin1String QBS_INSTALL_PREFIX("/home/bogdan/.local");
    const QLatin1String CXXFLAGS("cxxFlags");
    const QLatin1String CFLAGS("cFlags");
    const QLatin1String FRAMEWORKPATHS("frameworkPaths");
    const QLatin1String SYSTEM_FRAMEWORKPATHS("systemFrameworkPaths");
    const QLatin1String PRECOMPILEDHEADER("precompiledHeader");
}

QbsManager::QbsManager( QObject *parent, const QVariantList& args )
: KDevelop::AbstractFileManagerPlugin( "kdevqbsmanager", parent )
{
    Q_UNUSED(args)
    KDEV_USE_EXTENSION_INTERFACE( KDevelop::IBuildSystemManager )

    setXMLFile( "kdevqbsmanager.rc" );
}

QbsManager::~QbsManager()
{
}

KJob *QbsManager::install(ProjectBaseItem *item, const QUrl &specificPrefix)
{
    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return nullptr;

    QString root = specificPrefix.isEmpty() ? qbs::InstallOptions::defaultInstallRoot() : specificPrefix.toLocalFile();
    auto job = new QbsKJob([this, root, item]{
        qbs::InstallOptions opts;
        opts.setInstallRoot(root);
        opts.setLogElapsedTime(true);
        return m_projects[item->project()].project->installAllProducts(opts);
    }, it->setupParams.buildRoot(), this);

    m_currentOutputModel = job->model();

    connect(job, &QbsKJob::finished, this, [this, job, item]{
        if (job->error() == KJob::NoError)
            emit installed(item);
        else
            emit failed(item);
    });
    return job;
}

KJob *QbsManager::build(ProjectBaseItem *item)
{
    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return nullptr;

    auto job = new QbsKJob([this, item]{
        qbs::BuildOptions opts;
        opts.setMaxJobCount(qbs::BuildOptions::defaultMaxJobCount());
        opts.setInstall(false);
        opts.setEchoMode(qbs::defaultCommandEchoMode());
        opts.setLogElapsedTime(true);
        return m_projects[item->project()].project->buildAllProducts(opts);
    },  it->setupParams.buildRoot(), this);

    m_currentOutputModel = job->model();

    connect(job, &QbsKJob::finished, this, [this, job, item]{
        if (job->error() == KJob::NoError) {
            emit built(item);

            // workaround https://bugreports.qt.io/browse/QBS-901
            KDevelop::ProjectFolderItem *rootFolder = dynamic_cast<KDevelop::ProjectFolderItem *>(item);
            while(rootFolder->parent())
                rootFolder = dynamic_cast<KDevelop::ProjectFolderItem *>(rootFolder->parent());
            if (rootFolder)
                reload(rootFolder);
        } else {
            emit failed(item);
        }
    });
    return job;
}

KJob *QbsManager::clean(ProjectBaseItem *item)
{
    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return nullptr;

    auto job = new QbsKJob([this, item]{
        qbs::CleanOptions opts;
        opts.setLogElapsedTime(true);
        return  m_projects[item->project()].project->cleanAllProducts(opts);
    },  it->setupParams.buildRoot(), this);

    m_currentOutputModel = job->model();

    connect(job, &QbsKJob::finished, this, [this, job, item]{
        if (job->error() == KJob::NoError)
            emit cleaned(item);
        else
            emit failed(item);
    });
    return job;
}

KJob *QbsManager::configure(IProject */*project*/)
{
    return nullptr;
}

KJob *QbsManager::prune(IProject */*project*/)
{
    return nullptr;
}

QList<ProjectFolderItem *> QbsManager::parse(ProjectFolderItem *dom)
{
    return QList<ProjectFolderItem *>();
}

ProjectFolderItem *QbsManager::import(IProject *project)
{
    QDirIterator it(project->path().toLocalFile(), QStringList(QLatin1Literal("*.qbs")));
    qbs::SetupProjectParameters &setupParams = m_projects[project].setupParams;
    setupParams.setProjectFilePath(it.next());
    QbsProjectConfig conf(setupParams);
    if (conf.exec() != QDialog::Accepted)
        return nullptr;
    setupParams = conf.setupProjectParameters();
    qbs::Settings set(setupParams.settingsDirectory());
    const qbs::Preferences prefs(&set, setupParams.topLevelProfile());
    setupParams.setSearchPaths(prefs.searchPaths(QBS_INSTALL_PREFIX));
    setupParams.setPluginPaths(prefs.pluginPaths(QBS_INSTALL_PREFIX + QLatin1String("/lib")));
    setupParams.setLibexecPath(QBS_INSTALL_PREFIX + QLatin1String("/libexec"));

    return new QbsProjectFolder(this, project, project->path());
}

KJob *QbsManager::createImportJob(ProjectFolderItem *item)
{
    QbsProjectFolder *projectFolder = dynamic_cast<QbsProjectFolder *>(item);
    if (!projectFolder)
        return nullptr;

    const auto it = m_projects.find(item->project());
    if (it == m_projects.end())
        return nullptr;

    return new QbsImportJob(it->project, it->setupParams, this, projectFolder, this);
}

ProjectFileItem *QbsManager::addFile(const Path &file, ProjectFolderItem *parent)
{
    QbsData *data = dynamic_cast<QbsData *>(parent);
    if (!data || !data->groupData().isValid() || !data->productData().isValid())
        return nullptr;

    auto it = m_projects.find(parent->project());
    if (it ==m_projects.end())
        return nullptr;

    if (!KDevelop::createFile(file))
        return nullptr;

    auto err = it->project->addFiles(data->productData(), data->groupData(), QStringList(file.toLocalFile()));
    if (err.hasError()) {
        qCDebug(QBS) << "addFile failed" << err.toString();
        return nullptr;
    }

    // return a dummy file, the whole project files will be reloaded soon
    return new KDevelop::ProjectFileItem(parent->project(), file, parent);
}

bool QbsManager::removeFilesAndFolders(const QList<ProjectBaseItem *> &items)
{
    return false;
}

bool QbsManager::moveFilesAndFolders(const QList<ProjectBaseItem *> &items, ProjectFolderItem *newParent)
{
    return false;
}

bool QbsManager::copyFilesAndFolders(const Path::List &items, ProjectFolderItem *newParent)
{
    return false;
}

bool QbsManager::renameFile(ProjectFileItem *file, const Path &newPath)
{
    QbsData *dataPtr = dynamic_cast<QbsData *>(file->parent());
    if (!dataPtr || !dataPtr->groupData().isValid() || !dataPtr->productData().isValid())
        return false;

    QbsData data = *dataPtr;
    auto it = m_projects.find(file->project());
    if (it ==m_projects.end())
        return false;

    auto err = it->project->removeFiles(data.productData(), data.groupData(), QStringList(file->path().toLocalFile()));
    if (err.hasError()) {
        qCDebug(QBS) << "addFile failed" << err.toString();
        return false;
    }

    if (!KDevelop::renamePath(file->project(), file->path(), newPath)) {
        // add the file back to the project
        it->project->addFiles(data.productData(), data.groupData(), QStringList(file->path().toLocalFile()));
        return false;
    }

    err = it->project->addFiles(data.productData(), data.groupData(), QStringList(newPath.toLocalFile()));
    if (err.hasError()) {
        qCDebug(QBS) << "addFile failed" << err.toString();
        return false;
    }
    return true;
}

IProjectBuilder *QbsManager::builder() const
{
    return const_cast<QbsManager*>(this);
}

Path::List QbsManager::includeDirectories(ProjectBaseItem *item) const
{
    // incomplete keep an eye on https://bugreports.qt.io/browse/QBS-902
    Path::List paths;
    QbsData *qbsData = dynamic_cast<QbsData*>(item);
    if (!qbsData)
        return paths;
    const auto &props = qbsData->groupData().properties();
    auto list = props.getModulePropertiesAsStringList(CPP_MODULE, INCLUDEPATHS);
    list.append(props.getModulePropertiesAsStringList(CPP_MODULE, SYSTEM_INCLUDEPATHS));
    foreach (const QString &path, list)
        paths << Path(path);
    return paths;
}

QHash<QString, QString> QbsManager::defines(ProjectBaseItem *item) const
{
    // incomplete keep an eye on https://bugreports.qt.io/browse/QBS-902
    QHash<QString, QString> defs;
    QbsData *qbsData = dynamic_cast<QbsData*>(item);
    if (!qbsData)
        return defs;

    const auto &props = qbsData->groupData().properties();
    auto list = props.getModulePropertiesAsStringList(CPP_MODULE, DEFINES);
    foreach (const QString &def, list) {
        int pos = def.indexOf(QLatin1Char('='));
        if (pos >= 0)
            defs[def.left(pos)] = def.mid(pos +1);
        else
            defs[def] = QLatin1Char('1');
    }
    return defs;
}

bool QbsManager::hasIncludesOrDefines(ProjectBaseItem *item) const
{
    return dynamic_cast<QbsData*>(item);
}

ProjectTargetItem *QbsManager::createTarget(const QString &target, ProjectFolderItem *parent)
{
    return nullptr;
}

bool QbsManager::removeTarget(ProjectTargetItem *target)
{
    return false;
}

QList<ProjectTargetItem *> QbsManager::targets(ProjectFolderItem *) const
{
    return QList<ProjectTargetItem *>();
}

bool QbsManager::addFilesToTarget(const QList<ProjectFileItem *> &files, ProjectTargetItem *target)
{
    return false;
}

bool QbsManager::removeFilesFromTargets(const QList<ProjectFileItem *> &files)
{
    return false;
}

Path QbsManager::buildDirectory(ProjectBaseItem *item) const
{
    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return Path();

    return Path(it->project->projectData().buildDirectory());
}

bool QbsManager::reload(ProjectFolderItem *folder)
{
    qCDebug(QBS) << "reloading" << folder->path();

    IProject* project = folder->project();
    if (!project->isReady())
        return false;

    KJob *job = createImportJob(folder);
    if (!job)
        return false;
    project->setReloadJob(job);
    KDevelop::ICore::self()->runController()->registerJob( job );
    return true;
}

ProjectFolderItem *QbsManager::createFolderItem(IProject */*project*/, const Path &/*path*/, ProjectBaseItem */*parent*/)
{
    Q_ASSERT(false);
    return nullptr;
}

ProjectFileItem *QbsManager::createFileItem(IProject */*project*/, const Path &/*path*/, ProjectBaseItem */*parent*/)
{
    Q_ASSERT(false);
    return nullptr;
}

void QbsManager::doPrintWarning(const qbs::ErrorInfo &warning)
{
    // TODO show the warning
//    qDebug() << warning.toString();
}

void QbsManager::doPrintMessage(qbs::LoggerLevel level, const QString &message, const QString &tag)
{
    // TODO print the message elsewhere?
    if (m_currentOutputModel)
        m_currentOutputModel->appendLine(message);
//    qDebug() << level << tag << message;
}

#include "qbsmanager.moc"
