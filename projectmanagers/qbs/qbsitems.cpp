#include "qbsitems.h"
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <util/path.h>

static void setProjectWatcher(QbsProjectFolder *projectFolder, const QString &filePath)
{
    while(dynamic_cast<QbsProjectFolder *>(static_cast<KDevelop::ProjectBaseItem*>(projectFolder)->parent()))
        projectFolder = dynamic_cast<QbsProjectFolder *>(static_cast<KDevelop::ProjectBaseItem*>(projectFolder)->parent());

    auto *watcher = new QFileSystemWatcher(QStringList(filePath), projectFolder);
    QObject::connect(watcher, &QFileSystemWatcher::fileChanged, projectFolder, [projectFolder] {
        projectFolder->fileManager()->reload(projectFolder);
    });
}

class QbsProjectFile : public KDevelop::ProjectFileItem
{
public:
    QbsProjectFile(QbsProjectFolder *parent, const KDevelop::Path& path);
};

class QbsFile : public KDevelop::ProjectFileItem, public QbsData
{
public:
    QbsFile(const qbs::ProductData &productData, const qbs::GroupData &groupData, KDevelop::IProject* project, const KDevelop::Path& path, KDevelop::ProjectBaseItem* parent = nullptr);
};

class QbsProductFolder : public KDevelop::ProjectBuildFolderItem, public QbsData
{
public:
    QbsProductFolder(const qbs::ProductData &productData, KDevelop::ProjectBaseItem *parent);
};

class QbsGroupFolder : public KDevelop::ProjectFolderItem, public QbsData
{
public:
    QbsGroupFolder(const qbs::ProductData &productData, const qbs::GroupData &groupData, KDevelop::ProjectBaseItem *parent);
};

class QbsLibrary : public KDevelop::ProjectLibraryTargetItem
{
public:
    QbsLibrary(const qbs::TargetArtifact &artifact, QbsProductFolder *folder);
};

class QbsExecutable: public KDevelop::ProjectExecutableTargetItem
{
public:
    QbsExecutable(const qbs::TargetArtifact &artifact, QbsProductFolder *folder);

    // ProjectExecutableTargetItem interface
public:
    QUrl builtUrl() const override;
    QUrl installedUrl() const override;
};

static void collectFilesForProject(QbsProjectFolder *folder)
{
    const qbs::ProjectData &project = folder->projectData();
    new QbsProjectFile(folder, KDevelop::Path(project.location().filePath()));

    foreach (const qbs::ProjectData &subProject, project.subProjects())
        collectFilesForProject(new QbsProjectFolder(folder->fileManager(), subProject, subProject.name(), folder));

    foreach (const qbs::ProductData &product, project.products()) {
        if (product.location().filePath() != project.location().filePath())
            setProjectWatcher(folder, product.location().filePath());
        auto productFolder = new QbsProductFolder(product, folder);
        new KDevelop::ProjectFileItem(folder->project(), KDevelop::Path(product.location().toString()), productFolder);
        foreach (const qbs::TargetArtifact &ta, product.targetArtifacts()) {
            if (!ta.isValid())
                continue;

            if (ta.isExecutable())
                new QbsExecutable(ta, productFolder);
            else
                new QbsLibrary(ta, productFolder);
            qDebug() << ta.isValid() << ta.isExecutable() << ta.filePath() << ta.fileTags();
        }
        foreach (const qbs::GroupData &group, product.groups()) {
            QStringList allFiles(group.allFilePaths());
            KDevelop::ProjectFolderItem *groupFolder;
            if (group.name() == product.name() && product.location().toString() == group.location().toString()) {
                productFolder->setGroupData(group);
                groupFolder = productFolder;
            } else {
                groupFolder = new QbsGroupFolder(product, group, productFolder);
                new KDevelop::ProjectFileItem(folder->project(), KDevelop::Path(group.location().toString()), groupFolder);
            }
            foreach (const QString &file, allFiles)
                new QbsFile(product, group, folder->project(), KDevelop::Path(file), groupFolder);
        }
    }
}

QbsData::QbsData(const qbs::ProductData &productData, const qbs::GroupData &groupData)
    : m_productData(productData)
    , m_groupData(groupData)
{}

QbsProjectFolder::QbsProjectFolder(KDevelop::AbstractFileManagerPlugin *fileManager, KDevelop::IProject *project, const KDevelop::Path &path)
    : KDevelop::ProjectFolderItem(project, path)
    , m_fileManager(fileManager)
{}

QbsProjectFolder::QbsProjectFolder(KDevelop::AbstractFileManagerPlugin *fileManager, const qbs::ProjectData &projectData, const QString &name, KDevelop::ProjectBaseItem *parent)
    : KDevelop::ProjectFolderItem(name, parent)
    , m_fileManager(fileManager)
    , m_projectData(projectData)
{}

void QbsProjectFolder::collectItems()
{
    removeRows(0, rowCount());
    collectFilesForProject(this);
}

void QbsProjectFolder::setProjectData(const qbs::ProjectData &projectData)
{
    m_projectData = projectData;
}

QbsProjectFile::QbsProjectFile(QbsProjectFolder *parentFolder, const KDevelop::Path &path)
    : KDevelop::ProjectFileItem(parentFolder->project(), path, parentFolder)
{
    setProjectWatcher(parentFolder, path.toLocalFile());
}

QbsFile::QbsFile(const qbs::ProductData &productData, const qbs::GroupData &groupData, KDevelop::IProject *project, const KDevelop::Path &path, KDevelop::ProjectBaseItem *parent)
    : KDevelop::ProjectFileItem(project, path, parent)
    , QbsData(productData, groupData)
{}

QbsGroupFolder::QbsGroupFolder(const qbs::ProductData &productData, const qbs::GroupData &groupData, KDevelop::ProjectBaseItem *parent)
    : KDevelop::ProjectFolderItem(groupData.name(), parent)
    , QbsData(productData, groupData)
{
                                                                                                                                     // TODO uncomment me when this https://codereview.qt-project.org/#/c/140859/ patch is merged
    setPath(KDevelop::Path(QFileInfo(groupData.location().filePath()).absolutePath()/* + QLatin1Char('/') + groupData.prefix()*/));
    setText(groupData.name());
}

QbsProductFolder::QbsProductFolder(const qbs::ProductData &productData, KDevelop::ProjectBaseItem *parent)
    : KDevelop::ProjectBuildFolderItem(productData.name(), parent)
    , QbsData(productData, qbs::GroupData())
{
    setPath(KDevelop::Path(QFileInfo(productData.location().filePath()).absolutePath()));
    setText(productData.name());
}

QbsLibrary::QbsLibrary(const qbs::TargetArtifact &artifact, QbsProductFolder *folder)
    : KDevelop::ProjectLibraryTargetItem(folder->project(), QFileInfo(artifact.filePath()).fileName(), folder)
{
    setPath(KDevelop::Path(artifact.filePath()));
    setText(QFileInfo(artifact.filePath()).fileName()); // workaround https://bugreports.qt.io/browse/QBS-907
}

QbsExecutable::QbsExecutable(const qbs::TargetArtifact &artifact, QbsProductFolder *folder)
    : KDevelop::ProjectExecutableTargetItem(folder->project(), QFileInfo(artifact.filePath()).fileName(), folder)
{
    setPath(KDevelop::Path(artifact.filePath()));
    setText(QFileInfo(artifact.filePath()).fileName());// workaround https://bugreports.qt.io/browse/QBS-907
}

QUrl QbsExecutable::builtUrl() const
{
    return QUrl::fromLocalFile(path().toLocalFile());
}

QUrl QbsExecutable::installedUrl() const
{
    // workaround https://bugreports.qt.io/browse/QBS-906
    return QUrl::fromLocalFile(path().toLocalFile());
}

#include "qbsitems.moc"

