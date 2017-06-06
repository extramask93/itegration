#include "serialport.h"
#include "qasyncqueue.h"
#include <QDebug>
#include <QThread>
extern QAsyncQueue<QByteArray> queue;

SerialPort::SerialPort(QObject* parent):QThread(parent),port{}
{
    run_=true;
}
int SerialPort::writeS(const QByteArray& data)
{
    return port.write(data);
}

int SerialPort::write(const char *data)
{
    return port.write(data);
}

void SerialPort::run()
{
    while(run_)
    {
        while(!queue.isEmpty() && run_)
        {
            auto data = queue.pull();
            port.write(data);
            port.flush();
            QThread::msleep(300);
        }
        QThread::msleep(100);
    }
    run_=true;
}

bool SerialPort::isOpen() const
{
    return port.isOpen();
}
void SerialPort :: WriteSettings(QString name, int baudrate,int DataBits,
                         int Parity,int StopBits, int FlowControl){
    port.setPort(QSerialPortInfo(name));
    port.setBaudRate(baudrate);
    port.setDataBits((QSerialPort::DataBits)DataBits);
    port.setParity((QSerialPort::Parity)Parity);
    port.setStopBits((QSerialPort::StopBits)StopBits);
    port.setFlowControl((QSerialPort::FlowControl)FlowControl);
 }
void SerialPort::close()
{
    port.close();
}

bool SerialPort::open()
{
    return port.open(QIODevice::ReadWrite);
}
