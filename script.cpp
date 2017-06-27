#include "script.h"

Script::Script(QString path_, QObject *parent) :QObject(parent),path(path_)
{
    content=loadContent();
}

QStringList Script::loadContent()
{
    QStringList list;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("cant open the file");
    QTextStream in(&file);
    while(!in.atEnd())
        list.append(in.readLine());
    file.close();
    return list;
}

Script::Script(const Script &sc)
{
    path=sc.path;
    content=sc.content;
}
QString Script::getNameWithExtension()
{
    return QFileInfo(path).fileName();
}

QString Script::getPath()
{
    return path;
}

QString Script::getFileName()
{
    QString temp=getNameWithExtension();
    temp.chop(4);
    return temp;
}

QStringList Script::getContent()
{
    return content;
}

Script& Script::addCarrets()
{
    for(int i=0;i<content.size();i++) {
        content[i].append('\r');
    }
    return *this;
}

int Script::getNrOfLines()
{
    return content.size();
}

QStringList Script::getContentReadyToSend()
{
    number().addCarrets().addHeading();
    return content;
}
Script& Script::addHeading()
{
    content.push_front("ER\r");
    content.push_front("N \""+getFileName()+"\"\r");
    content.push_front("RS\r");
    return *this;
}
Script& Script::addLineBreaks()
{
    for(int i=0;i<content.size();i++)
        content[i]=content[i]+'\n';
    return *this;

}

Script& Script::number()
{
    int j=1;
    for(int i=0;i<content.size();i++) {
        content[i]=QString::number(j)+' '+content[i];
        j++;
    }
    return *this;
}
