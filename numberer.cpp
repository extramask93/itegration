#include "numberer.h"
#include <QFile>
#include <QTextStream>
#include<QDebug>

Numberer::Numberer(QObject *parent) : QObject(parent)
{

}
int Numberer::number(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;
    QTextStream in(&file);
    int i=1;
    while(!in.atEnd())
    {
        QString line=in.readLine();
        qDebug()<<i<<line; //TODO
        i++;
    }
    return 0;
}
