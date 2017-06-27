#include "serialport.h"
#include "qasyncqueue.h"
#include <QDebug>
#include <QThread>

SerialPort::SerialPort(QObject* parent):QThread(parent),port{}
{
    senddialog = new SendDialog(nullptr);
    connect(&port,SIGNAL(readyRead()),this,SLOT(read()));
    connect(senddialog,SIGNAL(cancelButtonClicked()),this,SLOT(cancelSending()));
}

SerialPort::~SerialPort()
{
    delete senddialog;
}
int SerialPort::writeS(const QByteArray& data)
{
    auto bytes=port.write(data);
    bool result=port.waitForBytesWritten(1000);
    if(!result)
    {
        emit writeTimeOut();
    }
    return bytes;
}
void SerialPort::read()
{
    static QByteArray message;
    char c;
    qint64 bytesRead=1;
    while(bytesRead!=-1 && bytesRead !=0)
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
int SerialPort::writeFile(Script script)
{
    if(script.getFileName().isEmpty())
        return 1;
    isSending=true;
    QStringList commands=script.getContentReadyToSend();
    int max=commands.size();
    int min=0;
    int current=0;
    senddialog->show();
    foreach (QString l, commands) {
       if(!isSending)
           break;
       writeS(l.toLocal8Bit());
       senddialog->updateProgressBar(max,min,current);
       current++;
       QThread::msleep(300);
       qApp->processEvents();
    }
    senddialog->updateProgressBar(max,min,max);
    senddialog->hide();
    return 0;
}

void SerialPort::cancelSending()
{
    isSending=false;
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
    bool result=port.open(QIODevice::ReadWrite);
    return result;
}
