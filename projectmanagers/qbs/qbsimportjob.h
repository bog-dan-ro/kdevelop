#ifndef QBSIMPORTJOB_H
#define QBSIMPORTJOB_H

#include "qbsitems.h"

#include <QPointer>
#include <QSharedPointer>

#include <KJob>

#include <api/project.h>
#include <logging/ilogsink.h>
#include <tools/setupprojectparameters.h>

class QbsImportJob : public KJob
{
    Q_OBJECT
public:
    explicit QbsImportJob(const QSharedPointer<qbs::Project>&qbsProject,
                          const qbs::SetupProjectParameters &params, qbs::ILogSink *logSink,
                          QbsProjectFolder *projectFolder, QObject *parent = 0);
    ~QbsImportJob();

    // KJob interface
public:
    void start() override;
    bool doKill() override;

    // ILogSink interface
private:
    void collectFilesForProject(const qbs::ProjectData &project, KDevelop::ProjectBaseItem *folder);

private:
    QSharedPointer<qbs::Project> m_qbsProject;
    qbs::SetupProjectParameters m_params;
    QbsProjectFolder *m_projectFolder;
    QPointer<qbs::SetupProjectJob> m_job;
    qbs::ILogSink *m_logSink;
};

#endif // QBSIMPORTJOB_H
