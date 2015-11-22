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

#include <KConfigGroup>
#include <algorithm>

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/iuicontroller.h>

#include <project/helper.h>
#include <project/projectmodel.h>

#include <util/executecompositejob.h>
#include <util/urlinfo.h>

using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(QBSSupportFactory, "kdevqbsmanager.json", registerPlugin<QbsManager>(); )

namespace {
    const QLatin1String CPP_MODULE("cpp");
    const QLatin1String DEFINES("defines");
    const QLatin1String COMPILER_DEFINES("compilerDefines");
    const QLatin1String INCLUDE_PATHS("includePaths");
    const QLatin1String SYSTEM_INCLUDEPATHS("systemIncludePaths");

    const QLatin1String CXXFLAGS("cxxFlags");
    const QLatin1String CFLAGS("cFlags");
    const QLatin1String FRAMEWORKPATHS("frameworkPaths");
    const QLatin1String SYSTEM_FRAMEWORKPATHS("systemFrameworkPaths");
    const QLatin1String PRECOMPILEDHEADER("precompiledHeader");

    const char SETUP_PARAMS_GRUP_KEY[] = "qbsSetupParams";
    const char PROJECT_FILE_PATH_KEY[] = "projectFilePath";
    const char TOP_LEVEL_PROFILE_KEY[] = "topLevelProfile";
    const char BUILD_ROOT_KEY[] = "buildRoot";
    const char BUILD_VARIANT_KEY[] = "buildVariant";
}

bool QbsManager::ProjectData::setPaths()
{
    qbs::Settings set(setupParams.settingsDirectory());
    const qbs::Preferences prefs(&set, setupParams.topLevelProfile());

    // TODO Check QBS_INSTALL_PREFIX ?
    setupParams.setSearchPaths(prefs.searchPaths(QLatin1String(QBS_INSTALL_PREFIX)));
    setupParams.setPluginPaths(prefs.pluginPaths(QLatin1String(QBS_INSTALL_PREFIX) + QLatin1String("/lib")));
    setupParams.setLibexecPath(QLatin1String(QBS_INSTALL_PREFIX)+ QLatin1String("/libexec"));
    return true;
}

bool QbsManager::ProjectData::setup(IProject *project)
{
    QDirIterator it(project->path().toLocalFile(), QStringList(QLatin1Literal("*.qbs")));
    setupParams.setProjectFilePath(it.next());
    if (!QFileInfo(setupParams.projectFilePath()).exists())
        return false;

    QbsProjectConfig conf(setupParams);
    if (conf.exec() != QDialog::Accepted)
        return false;

    setupParams = conf.setupProjectParameters();
    // save the qbs setup params
    KConfigGroup group = project->projectConfiguration()->group(SETUP_PARAMS_GRUP_KEY);
    group.writeEntry(PROJECT_FILE_PATH_KEY, setupParams.projectFilePath());
    group.writeEntry(TOP_LEVEL_PROFILE_KEY, setupParams.topLevelProfile());
    group.writeEntry(BUILD_ROOT_KEY, setupParams.buildRoot());
    group.writeEntry(BUILD_VARIANT_KEY, setupParams.buildVariant());
    group.sync();
    group.config()->sync();
    return setPaths();
}

bool QbsManager::ProjectData::configure(IProject *project)
{
    if (project->projectConfiguration()->hasGroup(SETUP_PARAMS_GRUP_KEY)) {
        KConfigGroup group = project->projectConfiguration()->group(SETUP_PARAMS_GRUP_KEY);
        setupParams.setProjectFilePath(group.readEntry(PROJECT_FILE_PATH_KEY));
        if (!QFileInfo(setupParams.projectFilePath()).exists())
            return setup(project);
        setupParams.setTopLevelProfile(group.readEntry(TOP_LEVEL_PROFILE_KEY));
        qbs::Settings set(setupParams.settingsDirectory());
        if (!set.profiles().contains(setupParams.topLevelProfile()))
            return setup(project);

        setupParams.setBuildRoot(group.readEntry(BUILD_ROOT_KEY));
        setupParams.setBuildVariant(group.readEntry(BUILD_VARIANT_KEY));
        return setPaths();
    }
    return setup(project);
}

QbsManager::QbsManager( QObject *parent, const QVariantList& args )
: AbstractFileManagerPlugin( "kdevqbsmanager", parent )
{
    Q_UNUSED(args)
    KDEV_USE_EXTENSION_INTERFACE( IBuildSystemManager )
    setXMLFile( "kdevqbsmanager.rc" );
}

QbsManager::~QbsManager()
{
}

KJob *QbsManager::install(ProjectBaseItem *item, const QUrl &specificPrefix)
{
    // workaround https://bugreports.qt.io/browse/QBS-901
    if (item->parent() || !dynamic_cast<ProjectFolderItem *>(item))
        return nullptr;

    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return nullptr;

    QList<KJob*>jobs;
    if (KJob *importJob = createImportJob(item->project()->projectItem()))
        jobs << importJob;

    QString root = specificPrefix.isEmpty() ? qbs::InstallOptions::defaultInstallRoot() : specificPrefix.toLocalFile();
    auto job = new QbsKJob([this, root, it]{
        qbs::InstallOptions opts;
        opts.setInstallRoot(root);
        opts.setLogElapsedTime(true);
        return it->project->installAllProducts(opts);
    }, it->setupParams.buildRoot(), this);

    m_currentOutputModel = job->model();

    connect(job, &QbsKJob::finished, this, [this, job, item]{
        if (job->error() == KJob::NoError)
            emit installed(item);
        else
            emit failed(item);
    });
    jobs << job;
    return new ExecuteCompositeJob(this, jobs);
}

KJob *QbsManager::build(ProjectBaseItem *item)
{
    // workaround https://bugreports.qt.io/browse/QBS-901
    if (item->parent() || !dynamic_cast<ProjectFolderItem *>(item))
        return nullptr;

    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return nullptr;

    QList<KJob*>jobs;
    if (KJob *importJob = createImportJob(item->project()->projectItem()))
        jobs << importJob;

    auto job = new QbsKJob([this, it]{
        qbs::BuildOptions opts;
        opts.setMaxJobCount(qbs::BuildOptions::defaultMaxJobCount());
        opts.setInstall(false);
        opts.setEchoMode(qbs::defaultCommandEchoMode());
        opts.setLogElapsedTime(true);
        return it->project->buildAllProducts(opts);
    },  it->setupParams.buildRoot(), this);

    m_currentOutputModel = job->model();

    connect(job, &QbsKJob::finished, this, [this, job, item]{
        if (job->error() == KJob::NoError) {
            emit built(item);

            // workaround https://bugreports.qt.io/browse/QBS-901
            ProjectFolderItem *rootFolder = dynamic_cast<ProjectFolderItem *>(item);
            while(rootFolder->parent())
                rootFolder = dynamic_cast<ProjectFolderItem *>(rootFolder->parent());
            if (rootFolder)
                reload(rootFolder);
        } else {
            emit failed(item);
        }
    });

    jobs << job;
    return new ExecuteCompositeJob(this, jobs);
}

KJob *QbsManager::clean(ProjectBaseItem *item)
{
    // workaround https://bugreports.qt.io/browse/QBS-901
    if (item->parent() || !dynamic_cast<ProjectFolderItem *>(item))
        return nullptr;

    auto it = m_projects.find(item->project());
    if (it ==m_projects.end())
        return nullptr;

    QList<KJob*>jobs;
    if (KJob *importJob = createImportJob(item->project()->projectItem()))
        jobs << importJob;

    auto job = new QbsKJob([this, it]{
        qbs::CleanOptions opts;
        opts.setLogElapsedTime(true);
        return  it->project->cleanAllProducts(opts);
    },  it->setupParams.buildRoot(), this);

    m_currentOutputModel = job->model();

    connect(job, &QbsKJob::finished, this, [this, job, item]{
        if (job->error() == KJob::NoError)
            emit cleaned(item);
        else
            emit failed(item);
    });
    jobs << job;
    return new ExecuteCompositeJob(this, jobs);
}

KJob *QbsManager::configure(IProject *project)
{
    return createImportJob(project->projectItem());
}

KJob *QbsManager::prune(IProject */*project*/)
{
    return nullptr;
}

QList<ProjectFolderItem *> QbsManager::parse(ProjectFolderItem */*dom*/)
{
    return QList<ProjectFolderItem *>();
}

ProjectFolderItem *QbsManager::import(IProject *project)
{
    if (!m_projects[project].configure(project))
        return nullptr;
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

    if (!createFile(file))
        return nullptr;

    auto err = it->project->addFiles(data->productData(), data->groupData(), QStringList(file.toLocalFile()));
    if (err.hasError()) {
        qCDebug(QBS) << "addFile failed" << err.toString();
        return nullptr;
    }

    // return a dummy file, the whole project files will be reloaded soon
    return new ProjectFileItem(parent->project(), file, parent);
}

bool QbsManager::removeFilesAndFolders(const QList<ProjectBaseItem *> &items)
{
    foreach (ProjectBaseItem *item, items) {
        auto it = m_projects.find(item->project());
        if (it ==m_projects.end())
            continue;

        QbsData *data = dynamic_cast<QbsData*>(item);
        if (!data)
            continue;

        Path itemPath = Path(parseUrl(item->path().toUrl()).url);
        if (dynamic_cast<ProjectFolderItem*>(item))
            it->project->removeGroup(data->productData(), data->groupData());
        else
            it->project->removeFiles(data->productData(), data->groupData(), QStringList(itemPath.toLocalFile()));

        QFileInfo fi(itemPath.toLocalFile());
        if (fi.exists())
            removePath(item->project(), itemPath, fi.isDir());
    }
    return true;
}

bool QbsManager::moveFilesAndFolders(const QList<ProjectBaseItem *> &items, ProjectFolderItem *newParent)
{
    Path::List itemsList;
    itemsList.reserve(items.size());
    foreach (ProjectBaseItem *item, items)
        itemsList.push_back(Path(parseUrl(item->path().toUrl()).url));

    return copyFilesAndFolders(itemsList, newParent) && removeFilesAndFolders(items);
}

bool QbsManager::copyFilesAndFolders(const Path::List &items, ProjectFolderItem *newParent)
{
    if (!QFileInfo(newParent->path().toLocalFile()).isDir())
        return false; // QbsGroup & QbsProduct sets ProjectFolderItem::patch correctly

    QbsData *data = dynamic_cast<QbsData *>(newParent);
    if (!data || !data->groupData().isValid() || !data->productData().isValid())
        return false;

    auto it = m_projects.find(newParent->project());
    if (it ==m_projects.end())
        return false;

    QStringList files;
    foreach (const Path &path, items) {
        Path filePath(parseUrl(path.toUrl()).url);
        QFileInfo inf(filePath.toLocalFile());
        if (!inf.isFile() && !inf.isDir())
            continue;

        if (copyPath(newParent->project(), filePath, newParent->path())) {
            if (inf.isFile())
                files << newParent->path().toLocalFile() + QLatin1Char('/') + filePath.lastPathSegment();
            else
                return false; //files << newParent->path().toLocalFile() + QLatin1Char('/') + filePath.lastPathSegment() + QLatin1String("/**");
        }
    }
    return !it->project->addFiles(data->productData(), data->groupData(), files).hasError();
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

    if (!renamePath(file->project(), file->path(), newPath)) {
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
    auto list = props.getModulePropertiesAsStringList(CPP_MODULE, INCLUDE_PATHS);
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
    list.append(props.getModulePropertiesAsStringList(CPP_MODULE, COMPILER_DEFINES));
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
    ICore::self()->runController()->registerJob( job );
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
    ICore::self()->uiController()->showErrorMessage(warning.toString());
}

void QbsManager::doPrintMessage(qbs::LoggerLevel level, const QString &message, const QString &tag)
{
    // TODO print the message elsewhere?
    if (m_currentOutputModel)
        m_currentOutputModel->appendLine(message);
    qCDebug(QBS) << level << tag << message;
}

#include "qbsmanager.moc"
