#include "interpreter.h"
#include "qasyncqueue.h"
#include <QDebug>
extern QAsyncQueue<QByteArray> queue;
Interpreter::Interpreter(IPrinter *console_,SerialPort *serial_,QObject *parent) : QObject(parent), checker()
{
    console=console_;
    serial=serial_;
}

Interpreter::~Interpreter()
{
}

bool Interpreter::isCheckingOn()
{
    return checker.isErrorCheckingOn();
}

void Interpreter::setCheckingOn(bool state)
{
    checker.setErrorCheckingOn(state);
    if(checker.isErrorCheckingOn())
        checking=true;
    else
        checking=false;
    emit changed();

}
void Interpreter::processCommand(QString command)
{
    if(command[0]=='!')
    {
        emit controlCommandIssued(command);
    }
    else
    {
        Result result = checker.checkLine(command);
        if(!result.errorCode)
        {
            auto commandByte = command.toLocal8Bit();
            serial->writeS(commandByte.append('\r'));
        }
        else
        {
            emit errorOccured(result.errorString);
        }
    }
}
int Interpreter::processScript(QString fileName)
{
    if(checker.checkFile(fileName))
    {
        emit errorOccured("Cannot open the file: "+fileName);
        return 1;
    }
    else
    {
        if(!isCheckingOn())
            return 0;
        foreach(auto result,checker.errorlist)
        {
            emit errorOccured("Line "+QString::number(result.lineNr)+": "+result.errorString);

        }
        if(!checker.errorlist.isEmpty())
            return 1;
        else
            return 0;
    }
}

void Interpreter::setOmmiting(bool state)
{
    checker.ommitUnknown=state;
}

void Interpreter::toggleChecker()
{
    if(isCheckingOn())
        setCheckingOn(false);
    else
        setCheckingOn(true);
}


