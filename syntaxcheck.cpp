#include "syntaxcheck.h"
#include "result.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <tuple>

SyntaxCheck::SyntaxCheck(QObject *parent) : QObject(parent){
    ommitUnknown=false;
    try{
        loadDatabase();
        setErrorCheckingOn(true);
    }
    catch(std::exception ex)
    {
        setErrorCheckingOn(false);
        QMessageBox::warning(nullptr,"Database error",ex.what(),QMessageBox::Ok);
    }
}
bool SyntaxCheck::isCommandFormat(QString line)
{
    QRegExp cmdPattern = createRegExp("^[A-Z]{2,5}.*($|\')");
    return hasMatch(line,cmdPattern);
}
bool SyntaxCheck::hasCorrectFootPrint(QString line)
{
    QRegExp cmdPattern = createRegExp("^[A-Z]{2,5}\\b");
    return hasMatch(line,cmdPattern);

}
bool SyntaxCheck::isFullCommandFormat(QString line,QString command)
{
    if(ommitUnknown)
        return 1;
    QRegExp fullPattern=createRegExp('^'+command+database[command]+R"*(\s*($|'))*");//R"(\s+)"
    return hasMatch(line,fullPattern);
}

QRegExp SyntaxCheck::createRegExp(QString pattern)
{
    return QRegExp(pattern,Qt::CaseSensitive);
}
bool SyntaxCheck::hasMatch(QString line, QRegExp &regexp)
{
    if(regexp.indexIn(line)==-1)
        return 0;
    else
        return 1;
}

QString SyntaxCheck::getCommandName(QString line)
{
    QRegExp cmdPattern = createRegExp("^\\w{2,5}\\b");
    return hasMatch(line,cmdPattern)?cmdPattern.capturedTexts()[0]:"";
}
bool SyntaxCheck::isAComment(QString line)
{
    if(line[0]=='\'')
        return 1;
    else
        return 0;
}
bool SyntaxCheck::isAControllCommand(QString line){
    if(line[0]=='!')
        return 1;
    else return 0;
}

void SyntaxCheck::setOmmitingDisabled(bool state)
{
    ommitUnknown=!state;
}
Result SyntaxCheck::checkAgainstDatabase(QString commandName)
{
    if(!database.contains(commandName))
        return Result(1,0,"Command not found in database");
    else
        return Result(0,0,"");
}

Result SyntaxCheck::checkLine(QString line)
{
   line=line.trimmed();
   if(!isErrorCheckingOn())
       return Result(0,0,"");
   if(line.isEmpty()||isAComment(line)||isAControllCommand(line))
       return Result(0,0,"");
   if(!isCommandFormat(line))
      return Result(1,0,"Unknown command format");
   if(!hasCorrectFootPrint(line))
        return Result(1,0,"Cannot find function footprint");
    QString commandName=getCommandName(line);
    Result a=checkAgainstDatabase(commandName);
    if(a.errorCode && !ommitUnknown)
        return a;
    if(isFullCommandFormat(line,commandName))
       return Result(0,0,"OK");
    else
       return Result(1,0,"Parameters not matching function footprint");
}
QStringList SyntaxCheck::readScript(QString filename)
{
    QStringList linesInFile;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
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
       linesInFile.append(line);
    }
    file.close();
    return linesInFile;
}

int SyntaxCheck::checkFile(QString filename)
{ 
    errorlist.clear();
    if(!isErrorCheckingOn())
        return 0;
    QStringList linesInFile = readScript(filename);
    int counterOfLines =0;
    foreach (auto line, linesInFile) {
        counterOfLines+=1;
        auto result = checkLine(line);
        if(result.errorCode)
        {
            result.lineNr=counterOfLines;
            errorlist.append(result);
        }

    }
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

bool SyntaxCheck::isOmmintingEnabled()
{
    return ommitUnknown;
}

bool SyntaxCheck::isErrorCheckingOn()
{
    return isOn;
}

void SyntaxCheck::setErrorCheckingOn(bool state)
{
    if(!state)
    {
        isOn=false;
        return;
    }
    try
    {
        loadDatabase();
    }
    catch(std::exception &ex)
    {
        isOn=false;
        QMessageBox::warning(nullptr,"Database error",ex.what(),QMessageBox::Ok);
        return;
    }
    isOn=state;
}


void SyntaxCheck::loadJson()
{
    QFile databaseFile(":/rc/database.txt");
    if (!databaseFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(std::string("Couldn't file containing syntax rules, by default no checking will take place."));
    }
    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();
    jsonDoc=QJsonDocument(QJsonDocument::fromJson(databaseData));
}

