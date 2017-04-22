#include "numberer.h"
#include <QFile>
#include <QTextStream>
#include<QDebug>

Numberer::Numberer(QObject *parent) : QObject(parent)
{

}
QStringList Numberer::number(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw ;
    QTextStream in(&file);
    QStringList result;
    int i=1;
    while(!in.atEnd())
    {
        QString line=in.readLine();
        line=QString::number(i)+line+'\r';
        i++;
        result.append(line);
    }
    return result;
}
