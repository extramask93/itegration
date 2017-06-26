#ifndef SYNTAXCHECK_H
#define SYNTAXCHECK_H

#include <QObject>
#include <QRegExp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHash>
#include <QMessageBox>
#include <exception>
#include "result.h"

class SyntaxCheck : public QObject
{
    Q_OBJECT
public:
    explicit SyntaxCheck(QObject *parent = 0);
    Result checkLine(QString line);
    int checkFile(QString filename);
    bool loadDatabase();
    bool isOmmintingEnabled();
    QList<Result> errorlist;
    bool isErrorCheckingOn();
    void setErrorCheckingOn(bool);
    bool ommitUnknown;
signals:
private:
    void loadJson();
    QString getCommandName(QString line);
    bool isCommandFormat(QString line);
    QJsonDocument jsonDoc;
    QHash<QString,QString> database;
    bool isOn;



    
    bool hasCorrectFootPrint(QString line);
    QRegExp createRegExp(QString pattern);
    bool hasMatch(QString line, QRegExp &regexp);
    bool isFullCommandFormat(QString line, QString command);
    QStringList readScript(QString filename);
    bool isAComment(QString line);
    Result checkAgainstDatabase(QString commandName);
    bool isAControllCommand(QString line);
public slots:
    void setOmmitingDisabled(bool state);
};

#endif // SYNTAXCHECK_H
