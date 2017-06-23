#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QMessageBox>
#include "bauddialog.h"
namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    Settings getSettings() const;
public slots:
        void apply();
        void showCustom(int index);

private:
    void saveSettings();
    void loadSettings();
    Ui::SettingsDialog *ui;
    void initFields();
    void initPortsInfo();
    void updateSettings();
    Settings currentSettings;
};

#endif // SETTINGSDIALOG_H
