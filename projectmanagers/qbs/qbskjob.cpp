#include "qbskjob.h"

#include <KLocalizedString>

#include <tools/processresult.h>

#include <outputview/outputdelegate.h>

QbsKJob::QbsKJob(const QbsKJob::CreateQbsJobFunction &function, const QString &buildDir, QObject *parent)
    : KDevelop::OutputJob(parent)
    , m_createQbsJobFunction(function)
    , m_job(nullptr)
{
    KDevelop::OutputModel* model = new KDevelop::OutputModel( QUrl::fromLocalFile(buildDir) );
    model->setFilteringStrategy( KDevelop::OutputModel::CompilerFilter );
    setModel(model);
    setCapabilities(Killable);

    setStandardToolView(KDevelop::IOutputView::BuildView);
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll );

    setObjectName(buildDir);
    setDelegate(new KDevelop::OutputDelegate);
    setKillJobOnOutputClose(true);
}

QbsKJob::~QbsKJob()
{
}

void QbsKJob::start()
{
    m_job = m_createQbsJobFunction();
    connect(m_job, &qbs::AbstractJob::finished, this, [this](bool success, qbs::AbstractJob *job) {
        if (!success) {
            setErrorText(job->error().toString());
            setError(KJob::UserDefinedError);
        } else {
            setError(KJob::NoError);
        }
        emitResult();
    });
    connect(m_job, &qbs::AbstractJob::taskStarted, this, [this](const QString &description, int maximumProgressValue, qbs::AbstractJob */*job*/){
        setToolTitle(description);
        setTitle(description);
        setObjectName(description);
        startOutput();
        emit KJob::description(this, description);
        setTotalAmount(Files, maximumProgressValue);
        setProcessedAmount(Files, 0);
    });
    connect(m_job, &qbs::AbstractJob::totalEffortChanged, this, [this](int totalEffort, qbs::AbstractJob */*job*/){
        setTotalAmount(Files, totalEffort);
    });
    connect(m_job, &qbs::AbstractJob::taskProgress, this, [this](int newProgressValue, qbs::AbstractJob */*job*/){
        setProcessedAmount(Files, newProgressValue);
    });

    if (qbs::BuildJob *buildJob = dynamic_cast<qbs::BuildJob *>(m_job)) {
        connect(buildJob, &qbs::BuildJob::reportCommandDescription, this, [this](const QString &/*highlight*/, const QString &message) {
            // TODO use highlight, possible values : compiling, linking
            model()->appendLine(message);
        });
        connect(buildJob, &qbs::BuildJob::reportProcessResult, this, [this](const qbs::ProcessResult &result) {
            model()->appendLines(result.stdOut());
            model()->appendLines(result.stdErr());
    });
    }
}

KDevelop::OutputModel *QbsKJob::model()
{
    return static_cast<KDevelop::OutputModel *>(OutputJob::model());
}

bool QbsKJob::doKill()
{
    if (m_job)
        m_job->cancel();
    return true;
}

#include "qbskjob.moc"
