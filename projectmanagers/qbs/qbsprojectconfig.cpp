#include "qbsprojectconfig.h"
#include "ui_qbsprojectconfig.h"

#include <QFileInfo>

#include <tools/settings.h>

QbsProjectConfig::QbsProjectConfig(const qbs::SetupProjectParameters &setupProjectParameters, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QbsProjectConfig),
    m_setupProjectParameters(setupProjectParameters)
{
    ui->setupUi(this);

    qbs::Settings set(setupProjectParameters.settingsDirectory());
    ui->profiles->addItems(set.profiles());

    if (m_setupProjectParameters.topLevelProfile().isEmpty())
        m_setupProjectParameters.setTopLevelProfile(set.defaultProfile());
    ui->profiles->setCurrentText(m_setupProjectParameters.topLevelProfile());
    connect(ui->profiles, &QComboBox::currentTextChanged, this, [this](const QString &profile){
        m_setupProjectParameters.setTopLevelProfile(profile);
    });

    if (m_setupProjectParameters.buildRoot().isEmpty()) {
        QFileInfo fi(m_setupProjectParameters.projectFilePath());
        QDir dir(fi.absoluteDir());
        dir.cdUp();
        m_setupProjectParameters.setBuildRoot(dir.absolutePath() + QLatin1Char('/') + fi.baseName() + QLatin1Literal("-build"));
    }
    ui->buildDir->setUrl(QUrl::fromLocalFile(m_setupProjectParameters.buildRoot()));
    connect(ui->buildDir, &KUrlRequester::textChanged, this, [this](const QString &path){
        m_setupProjectParameters.setBuildRoot(path);
    });

    if (m_setupProjectParameters.buildVariant().isEmpty())
        m_setupProjectParameters.setBuildVariant(ui->buildType->currentText());
    else
        ui->buildType->setCurrentText(m_setupProjectParameters.buildVariant());
    connect(ui->buildType, &QComboBox::currentTextChanged, this, [this](const QString&type){
        m_setupProjectParameters.setBuildVariant(type);
    });
}

QbsProjectConfig::~QbsProjectConfig()
{
    delete ui;
}

#include "qbsprojectconfig.moc"
