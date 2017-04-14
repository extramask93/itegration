#include "syntaxcheck.h"
#include "result.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <tuple>

SyntaxCheck::SyntaxCheck(QObject *parent) : QObject(parent){
   loadDatabase();

}
Result SyntaxCheck::checkLine(QString line,bool ommitUnknown)
{
   line = line.trimmed();

   QString cmdPattern = "^\\w{2,5}\\s+[0-9A-Za-z,]*\\s*($|\')";
   QRegExp cmdrx1(cmdPattern,Qt::CaseInsensitive);
   if(cmdrx1.indexIn(line)==-1)
      return Result(1,0,"Unknown command format");

    cmdPattern = "^\\w{2,5}\\b";
    QRegExp cmdrx(cmdPattern,Qt::CaseInsensitive);
    if(cmdrx.indexIn(line)==-1)
        return Result(1,0,"Cannot find function footprint");
    if(cmdrx.captureCount()>1)
        return Result(1,0,"Only one command per line allowed");
    QString key = cmdrx.capturedTexts()[0];
    if(!database.contains(key))
    {
        if(ommitUnknown)
            return Result(0,0,"Command not found in datbase(ommited)");
        else
            return Result(1,0,"Command not found in database");
    }

    QString fullPattern='^'+key+"\\s+"+database[key]+"\\s*($|\')";
    QRegExp fullrx(fullPattern,Qt::CaseInsensitive);
    if(fullrx.indexIn(line)!=-1)
       return Result(0,0,"OK");
    else
       return Result(1,0,"Parameters not matching function footprint");
}
void SyntaxCheck::checkFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return;}
    QTextStream in(&file);
    int counter =0;
    while (!in.atEnd())
    {
       counter+=1;
       QString line = in.readLine();
       line=line.trimmed();
       if(line[0]=='!'||line[0]=='\'')
           continue;
       auto result = checkLine(line,1);
       if(result.errorCode)
       {
           result.lineNr=counter;
           errorlist.append(result);
       }
    }
    file.close();
}

bool SyntaxCheck::loadDatabase()
{
    if(!loadJson())
        return false;
    QJsonObject json(jsonDoc.object());
    QStringList values;
    foreach(QString key,json.keys())
    {
        database.insert(key,json[key].toString());
    }
    return true;
}

bool SyntaxCheck::loadJson()
{
    QFile databaseFile(":/rc/database.txt");
    if (!databaseFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();
    jsonDoc=QJsonDocument(QJsonDocument::fromJson(databaseData));
    return true;
}

