#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCoreApplication>
#include "senddialog.h"
#include "numberer.h"
#include "script.h"
#include <QObject>
#include <QThread>

class SerialPort : public QThread
{
    Q_OBJECT
public:
    QSerialPort port;
    SendDialog *senddialog;
    bool isSending;
    SerialPort(QObject* parent);
    ~SerialPort();
    bool isOpen() const;
    void WriteSettings(QString name, int baudrate,int DataBits,int Parity,int StopBits, int FlowControl);
    void close();
    bool open();
public slots:
    int writeS(const QByteArray &data);
    void read();
    int writeFile(Script script);
    void cancelSending();
signals:
    void writeTimeOut();
    void messageArrived(QByteArray message);
};

#endif // SERIALPORT_H
