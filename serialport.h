#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QThread>

class SerialPort : public QThread
{
    Q_OBJECT
    volatile bool run_;

public:
    QSerialPort port;
    SerialPort(QObject* parent);
    void run() override;
    bool isOpen() const;
    void WriteSettings(QString name, int baudrate,int DataBits,int Parity,int StopBits, int FlowControl);
    void close();
    bool open();

public slots:
    int writeS(const QByteArray &data);
    int write(const char *data);
    void read();
signals:
    void writeTimeOut();
    void messageArrived(QByteArray message);
};

#endif // SERIALPORT_H
