#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QSerialPort>
#include <QObject>

class SerialPort : public QSerialPort
{
    Q_OBJECT
public:
    SerialPort(QObject* parent);
public slots:
    int writeS(const QByteArray &data);
};

#endif // SERIALPORT_H
