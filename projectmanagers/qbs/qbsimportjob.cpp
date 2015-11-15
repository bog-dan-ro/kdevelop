#include "qbsimportjob.h"

#include "qbsitems.h"

#include <QDir>
#include <QThread>
#include <api/jobs.h>
#include <tools/installoptions.h>
#include <interfaces/iproject.h>
#include <util/path.h>

QbsImportJob::QbsImportJob(const QSharedPointer<qbs::Project> &qbsProject, const qbs::SetupProjectParameters &params, qbs::ILogSink *logSink, QbsProjectFolder *projectFolder, QObject *parent)
    : KJob(parent)
    , m_qbsProject(qbsProject)
    , m_params(params)
    , m_projectFolder(projectFolder)
    , m_logSink(logSink)
{
    QDir(params.buildRoot()).mkpath(params.buildRoot());
}

QbsImportJob::~QbsImportJob()
{
}

void QbsImportJob::start()
{
    m_job = m_qbsProject->setupProject(m_params, m_logSink, this);
    connect(m_job.data(), &qbs::SetupProjectJob::finished, this, [this](bool success, qbs::AbstractJob *job) {
        if (!success) {
            setErrorText(job->error().toString());
            setError(KJob::UserDefinedError);
        } else {
            setError(KJob::NoError);
            *m_qbsProject = static_cast<qbs::SetupProjectJob*>(job)->project();
            m_projectFolder->setProjectData(m_qbsProject->projectData());
            m_projectFolder->collectItems();
        }
        emitResult();
    });
    connect(m_job.data(), &qbs::SetupProjectJob::taskStarted, this, [this](const QString &description, int maximumProgressValue, qbs::AbstractJob */*job*/) {
        emit KJob::description(this, description);
        setTotalAmount(Files, maximumProgressValue);
        setProcessedAmount(Files, 0);
    });
    connect(m_job.data(), &qbs::SetupProjectJob::totalEffortChanged, this, [this](int totalEffort, qbs::AbstractJob */*job*/){
        setTotalAmount(Files, totalEffort);
    });
    connect(m_job.data(), &qbs::SetupProjectJob::taskProgress, this, [this](int newProgressValue, qbs::AbstractJob */*job*/){
        setProcessedAmount(Files, newProgressValue);
    });
}

bool QbsImportJob::doKill()
{
    if (m_job) {
        m_job->cancel();
        return m_job->state() != qbs::AbstractJob::StateRunning;
    }
    return false;
}

#include "qbsimportjob.moc"
