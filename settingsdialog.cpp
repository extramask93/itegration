#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QSettings>
QT_USE_NAMESPACE
SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    initFields();
    initPortsInfo();
    QWidget::connect(ui->okButton,SIGNAL (clicked()),this,SLOT (apply()));
    QWidget::connect(ui->cancelButton,SIGNAL (clicked()),this,SLOT (hide()));
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::saveSettings()
{
    QSettings settings_("Jozek","Robots");
    settings_.beginGroup("Settings");
    settings_.remove("");
    settings_.setValue("Port",ui->portBox->currentIndex());
    settings_.setValue("Parity",ui->parityBox->currentIndex());
    settings_.setValue("StopBits",ui->stopBox->currentIndex());
    settings_.setValue("Baud",ui->baudBox->currentIndex());
    settings_.setValue("DataBits",ui->dataBox->currentIndex());
    settings_.setValue("Flow",ui->flowBox->currentIndex());
    settings_.setValue("Port",ui->portBox->currentIndex());
    settings_.endGroup();
    qDebug() << settings_.fileName();
}
void SettingsDialog::loadSettings()
{
    QSettings settings_("Jozek","Robots");
    settings_.beginGroup("Settings");
    ui->portBox->setCurrentIndex(settings_.value("Port").toInt());
    ui->parityBox->setCurrentIndex(settings_.value("Parity").toInt());
    ui->stopBox->setCurrentIndex(settings_.value("StopBits").toInt());
    ui->baudBox->setCurrentIndex(settings_.value("Baud").toInt());
    ui->dataBox->setCurrentIndex(settings_.value("DataBits").toInt());
    ui->flowBox->setCurrentIndex(settings_.value("Flow").toInt());
    ui->portBox->setCurrentIndex(settings_.value("Port").toInt());
    updateSettings();

}

void SettingsDialog::apply()
{
    updateSettings();
    saveSettings();
    hide();
}
void SettingsDialog::updateSettings()
{
    if(ui->baudBox->currentIndex()==4)
        currentSettings.baudRate = ui->baudBox->currentText().toInt();
     else
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudBox->itemData(ui->baudBox->currentIndex()).toInt());
     currentSettings.stringBaudRate=QString::number(currentSettings.baudRate);

     currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                 ui->dataBox->itemData(ui->dataBox->currentIndex()).toInt());
     currentSettings.stringDataBits = ui->dataBox->currentText();

     currentSettings.parity = static_cast<QSerialPort::Parity>(
                 ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
     currentSettings.stringParity = ui->parityBox->currentText();

     currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                 ui->stopBox->itemData(ui->stopBox->currentIndex()).toInt());
     currentSettings.stringStopBits = ui->stopBox->currentText();

     currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                 ui->flowBox->itemData(ui->flowBox->currentIndex()).toInt());
     currentSettings.stringFlowControl = ui->flowBox->currentText();
     currentSettings.name=ui->portBox->currentText();
}

void SettingsDialog::initFields()
{
    ui->parityBox->addItem(tr("None"),QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Odd"),QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Even"),QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Mark"),QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"),QSerialPort::SpaceParity);

    ui->stopBox->addItem("1",QSerialPort::OneStop);
    ui->stopBox->addItem("1.5",QSerialPort::OneAndHalfStop);
    ui->stopBox->addItem("2",QSerialPort::TwoStop);

    ui->baudBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudBox->addItem(tr("Custom"));

    ui->dataBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBox->setCurrentIndex(3);

    ui->flowBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

}
void SettingsDialog::initPortsInfo()
{
   ui->portBox->clear();
   const auto infos = QSerialPortInfo::availablePorts();
   foreach (const QSerialPortInfo &info, infos) {
       ui->portBox->addItem(info.portName());
   }
}
