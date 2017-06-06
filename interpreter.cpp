#include "interpreter.h"
#include "qasyncqueue.h"
#include <QDebug>
extern QAsyncQueue<QByteArray> queue;
Interpreter::Interpreter(IPrinter *console_,QObject *parent) : QObject(parent), checker()
{
    console=console_;
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


