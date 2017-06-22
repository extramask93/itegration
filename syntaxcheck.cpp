#include "syntaxcheck.h"
#include "result.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <tuple>

SyntaxCheck::SyntaxCheck(QObject *parent) : QObject(parent){
    try{
        loadDatabase();
    }
    catch(std::exception ex)
    {
        setOn(false);
        QMessageBox::warning(nullptr,"Database error",ex.what(),QMessageBox::Ok);
    }

}
Result SyntaxCheck::checkLine(QString line,bool ommitUnknown)
{
   if(!isOn())
       return Result(0,0,"Ommited");
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

    QString fullPattern='^'+key+R"(\s+)"+database[key]+R"*(\s*($|'))*";
    qDebug()<<"fullpattern: ";
    qDebug()<<fullPattern;
    QRegExp fullrx(fullPattern,Qt::CaseInsensitive);
    if(fullrx.indexIn(line)!=-1)
       return Result(0,0,"OK");
    else
       return Result(1,0,"Parameters not matching function footprint");
}
int SyntaxCheck::checkFile(QString filename)
{
    if(!isOn())
        return 0;
    errorlist.clear();
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error("Can not open the file for compilation");
    }
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
    return 0;
}

bool SyntaxCheck::loadDatabase()
{
    try
    {
        loadJson();
    }
    catch(std::exception &ex)
    {
        throw;
    }
    QJsonObject json(jsonDoc.object());
    QStringList values;
    database.clear();
    foreach(QString key,json.keys())
    {
        database.insert(key,json[key].toString());
    }
    return true;
}

bool SyntaxCheck::isOn()
{
    return on;
}

void SyntaxCheck::setOn(bool state)
{
    if(!state)
    {
        on=false;
        return;
    }
    try
    {
        loadDatabase();
    }
    catch(std::exception &ex)
    {
        on=false;
        QMessageBox::warning(nullptr,"Database error",ex.what(),QMessageBox::Ok);
        return;
    }
    on=state;
    ///
//    bool filled = false;
//    for(int i=1;i<=8;i++)
//    {
//        for(int j=1;j<=8;j++)
//        {
//            rectangle.draw(filled);
//            rectangle.moveToTheNextColumn();
//            filled=!filled;
//        }
//        filled=!filled;
//        rectangle.moveToTheNextRow();
//    }
    //
}


bool SyntaxCheck::loadJson()
{
    QFile databaseFile(":/rc/database.txt");
    if (!databaseFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(std::string("Couldn't file containing syntax rules, by default no checking will take place."));
    }
    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();
    jsonDoc=QJsonDocument(QJsonDocument::fromJson(databaseData));
    return true;
}

