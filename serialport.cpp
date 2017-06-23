#include "serialport.h"
#include "qasyncqueue.h"
#include <QDebug>
#include <QThread>
extern QAsyncQueue<QByteArray> queue;

SerialPort::SerialPort(QObject* parent):QThread(parent),port{}
{
    run_=true;
    connect(&port,SIGNAL(readyRead()),this,SLOT(read()));
}
int SerialPort::writeS(const QByteArray& data)
{
    bool isop=port.isOpen();
    auto bytes=port.write(data);
    bool result=port.waitForBytesWritten(1000);
    if(!result)
    {
        emit writeTimeOut();
        queue.clean();
    }
    return bytes;
}
void SerialPort::read()
{
    static QByteArray message;
    char c;
    qint64 bytesRead=1;
    while(bytesRead!=-1 and bytesRead !=0)
    {
        bytesRead=port.read(&c,1);
        message.append(c);
        if(c=='\r')
        {
            emit messageArrived(message);
            message.clear();
            c=' ';
        }
    }
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
            writeS(data);
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
    run_=false;
    queue.clean();
    QThread::wait(5000);
    port.close();
}

bool SerialPort::open()
{
    bool result=port.open(QIODevice::ReadWrite);
    if(result)
    {
        run_=true;
        start();
    }
    return result;
}
