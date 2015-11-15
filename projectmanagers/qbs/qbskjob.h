#ifndef QBSKJOB_H
#define QBSKJOB_H

#include <api/jobs.h>

#include <outputview/outputjob.h>
#include <outputview/outputmodel.h>
#include <functional>

class QbsKJob : public KDevelop::OutputJob
{
    Q_OBJECT
public:
    typedef std::function<qbs::AbstractJob *()> CreateQbsJobFunction;
    QbsKJob(const CreateQbsJobFunction &function, const QString& buildDir, QObject *parent = 0);
    ~QbsKJob();
    KDevelop::OutputModel *model();

    // KJob interface
public:
    void start() override;

    // KJob interface
protected:
    bool doKill() override;

private:
    CreateQbsJobFunction m_createQbsJobFunction;
    qbs::AbstractJob *m_job;
};

#endif // QBSKJOB_H
