#include "interpreter.h"
#include "qasyncqueue.h"
#include <QDebug>
extern QAsyncQueue<QByteArray> queue;
Interpreter::Interpreter(IPrinter *console_,QObject *parent) : QObject(parent), checker()
{
    console=console_;
}

bool Interpreter::isCheckingOn()
{
    return checker.isOn();
}

void Interpreter::setCheckingOn(bool state)
{
    checker.setOn(state);
    if(checker.isOn())
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
        Result result = checker.checkLine(command,1);
        if(!result.errorCode)
        {
            auto commandByte = command.toLocal8Bit();
            queue.push(commandByte.append('\r'));
        }
        else
        {
            console->printError(result.errorString);
        }
    }
}
int Interpreter::processScript(QString fileName)
{
    if(checker.checkFile(fileName))
    {
        console->printError("Cannot open the file: "+fileName);
        return 1;
    }
    else
    {
        if(!isCheckingOn())
            return 0;
        foreach(auto result,checker.errorlist)
        {
            console->printError("Line "+QString::number(result.lineNr)+": "+result.errorString);

        }
        if(!checker.errorlist.empty())
            return 1;
        else
            return 0;
    }
}

void Interpreter::toggleChecker()
{
    if(isCheckingOn())
        setCheckingOn(false);
    else
        setCheckingOn(true);
}


