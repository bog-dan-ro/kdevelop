/* KDevelop QBS Support
 *
 * Copyright 2015 BogDan Vatra <bogdan@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef QBSMANAGER_H
#define QBSMANAGER_H

#include <outputview/outputmodel.h>
#include <project/abstractfilemanagerplugin.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <project/interfaces/iprojectbuilder.h>

#include <qbs.h>

#include <QScopedPointer>


class QbsManager : public KDevelop::AbstractFileManagerPlugin,
        public KDevelop::IProjectBuilder,
        public KDevelop::IBuildSystemManager,
        public qbs::ILogSink
{
    Q_OBJECT
    Q_INTERFACES( KDevelop::IProjectBuilder )
    Q_INTERFACES( KDevelop::IProjectFileManager )
    Q_INTERFACES( KDevelop::IBuildSystemManager )

public:
    explicit QbsManager( QObject *parent = NULL, const QVariantList& args = QVariantList()  );
    virtual ~QbsManager();

    // IProjectFileManager interface
public:
    Features features() const override { return Features(Folders | /*Targets | */Files); }
    QList<KDevelop::ProjectFolderItem *> parse(KDevelop::ProjectFolderItem *dom) override;
    KDevelop::ProjectFolderItem *import(KDevelop::IProject *project) override;
    KJob *createImportJob(KDevelop::ProjectFolderItem *item) override;
    KDevelop::ProjectFileItem *addFile(const KDevelop::Path &file, KDevelop::ProjectFolderItem *parent) override;
    bool removeFilesAndFolders(const QList<KDevelop::ProjectBaseItem *> &items) override;
    bool moveFilesAndFolders(const QList<KDevelop::ProjectBaseItem *> &items, KDevelop::ProjectFolderItem *newParent) override;
    bool copyFilesAndFolders(const KDevelop::Path::List &items, KDevelop::ProjectFolderItem *newParent) override;
    bool renameFile(KDevelop::ProjectFileItem *file, const KDevelop::Path &newPath) override;

    // IProjectBuilder interface
public:
    KJob *install(KDevelop::ProjectBaseItem *item, const QUrl &specificPrefix) override;
    KJob *build(KDevelop::ProjectBaseItem *item) override;
    KJob *clean(KDevelop::ProjectBaseItem *item) override;
    KJob *configure(KDevelop::IProject *project) override;
    KJob *prune(KDevelop::IProject *project) override;
signals:
    void built( KDevelop::ProjectBaseItem *dom );
    void installed( KDevelop::ProjectBaseItem* );
    void cleaned( KDevelop::ProjectBaseItem* );
    void failed( KDevelop::ProjectBaseItem *dom );
    void configured( KDevelop::IProject* );
    void pruned( KDevelop::IProject* );

    // IBuildSystemManager interface
public:
    KDevelop::IProjectBuilder *builder() const override;
    KDevelop::Path::List includeDirectories(KDevelop::ProjectBaseItem *item) const override;
    QHash<QString, QString> defines(KDevelop::ProjectBaseItem *item) const override;
    bool hasIncludesOrDefines(KDevelop::ProjectBaseItem *item) const override;
    KDevelop::ProjectTargetItem *createTarget(const QString &target, KDevelop::ProjectFolderItem *parent) override;
    bool removeTarget(KDevelop::ProjectTargetItem *target) override;
    QList<KDevelop::ProjectTargetItem *> targets(KDevelop::ProjectFolderItem *) const override;
    bool addFilesToTarget(const QList<KDevelop::ProjectFileItem *> &files, KDevelop::ProjectTargetItem *target) override;
    bool removeFilesFromTargets(const QList<KDevelop::ProjectFileItem *> &files) override;
    KDevelop::Path buildDirectory(KDevelop::ProjectBaseItem *item) const override;
    bool reload(KDevelop::ProjectFolderItem *item) override;

    // AbstractFileManagerPlugin interface
protected:
    KDevelop::ProjectFolderItem *createFolderItem(KDevelop::IProject *project, const KDevelop::Path &path, KDevelop::ProjectBaseItem *parent) override;
    KDevelop::ProjectFileItem *createFileItem(KDevelop::IProject *project, const KDevelop::Path &path, KDevelop::ProjectBaseItem *parent) override;

    // ILogSink interface
private:
    void doPrintWarning(const qbs::ErrorInfo &warning) override;
    void doPrintMessage(qbs::LoggerLevel level, const QString &message, const QString &tag) override;

private:
    class ProjectData {
    public:
        ProjectData()
            : project(new qbs::Project) {}

        bool configure(KDevelop::IProject *project);
        QSharedPointer<qbs::Project> project;
        qbs::SetupProjectParameters setupParams;

    private:
        bool setPaths();
        bool setup(KDevelop::IProject *project);
    };

    QHash<KDevelop::IProject *, ProjectData> m_projects;
    QPointer<KDevelop::OutputModel> m_currentOutputModel;
};
#endif
