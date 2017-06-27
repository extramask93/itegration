#include "logger.h"

Logger::Logger(QObject *parent) : QObject(parent)
{

}

void Logger::log(QString str)
{
    while(iolist.size()>=255)
        iolist.pop_back();
    iolist.append(str);
}

QStringList Logger::loadSession(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in(&file);
    QStringList list;
    QString buffer;
    while(!in.atEnd())
    {
        in>>buffer;
        buffer.trimmed();
        list.append(buffer);
    }
    while(list.size()>maxNrOfLogEntries)
        list.pop_back();
    file.close();
    return list;

}

void Logger::addToSession(QString message)
{
    iolist.append(message);
}
void Logger::saveCommands(QStringList list,QString fileName)
{
    while(list.size()>maxNrOfHistoryEntries)
        list.pop_back();
    QFile file(fileName);
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&file);
    foreach (auto str, list) {
        out<<str<<endl;
    }
    file.close();
}

void Logger::saveSession(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&file);
    foreach (auto str, iolist) {
        out<<str<<endl;
    }
    file.close();
}

QStringList Logger::loadCommands(QString fileName)
{
    QStringList list;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in(&file);
    while(!in.atEnd())
        list.append(in.readLine());
    while(list.size()>maxNrOfHistoryEntries)
        list.pop_back();
    file.close();
    return list;
}


