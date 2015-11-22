#ifndef QBSITEMS_H
#define QBSITEMS_H

#include <project/abstractfilemanagerplugin.h>
#include <project/projectmodel.h>
#include <api/projectdata.h>

class QbsData
{
public:
    QbsData(const qbs::ProductData &productData, const qbs::GroupData &groupData);

    const qbs::ProductData &productData() const { return m_productData; }
    const qbs::GroupData &groupData() const { return m_groupData; }
    void setGroupData(const qbs::GroupData &groupData) { m_groupData = groupData; }

protected:
    qbs::ProductData m_productData;
    qbs::GroupData m_groupData;
};

class QbsGroupFolder : public KDevelop::ProjectFolderItem, public QbsData
{
public:
    QbsGroupFolder(const qbs::ProductData &productData, const qbs::GroupData &groupData, KDevelop::ProjectBaseItem *parent);
};

class QbsProjectFolder : public QObject, public KDevelop::ProjectFolderItem
{
    Q_OBJECT
public:
    QbsProjectFolder(KDevelop::AbstractFileManagerPlugin *fileManager, KDevelop::IProject* project, const KDevelop::Path& path);
    QbsProjectFolder(KDevelop::AbstractFileManagerPlugin *fileManager, const qbs::ProjectData &projectData, const QString& name, ProjectBaseItem *parent);

    void collectItems();
    KDevelop::AbstractFileManagerPlugin *fileManager() const { return m_fileManager; }
    const qbs::ProjectData &projectData() const { return m_projectData; }
    void setProjectData(const qbs::ProjectData &projectData);

private:
    KDevelop::AbstractFileManagerPlugin *m_fileManager;
    qbs::ProjectData m_projectData;
};

#endif // QBSITEMS_H
