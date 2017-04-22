#include "serialport.h"
#include <QDebug>

SerialPort::SerialPort(QObject* parent):QSerialPort(parent)
{

}
int SerialPort::writeS(const QByteArray& data)
{
    return write(data);
}

