#ifndef QBSPROJECTCONFIG_H
#define QBSPROJECTCONFIG_H

#include <QDialog>

#include <tools/setupprojectparameters.h>

namespace Ui {
class QbsProjectConfig;
}

class QbsProjectConfig : public QDialog
{
    Q_OBJECT

public:
    explicit QbsProjectConfig(qbs::SetupProjectParameters &setupProjectParameters, QWidget *parent = 0);
    ~QbsProjectConfig();
    qbs::SetupProjectParameters &setupProjectParameters() const { return m_setupProjectParameters; }

private:
    Ui::QbsProjectConfig *ui;
    qbs::SetupProjectParameters &m_setupProjectParameters;
};

#endif // QBSPROJECTCONFIG_H
