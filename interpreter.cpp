#include "interpreter.h"
#include <QDebug>

Interpreter::Interpreter(QObject *parent) : QObject(parent)
{

}
void Interpreter::processCommand(QString command)
{
    auto commandByte = command.toLocal8Bit();
    if(commandByte[0]=='!')
    {
        emit controlCommandIssued(commandByte);
    }
    else
    {
        emit robotCommandIssued(commandByte.append('\r'));
    }
}

