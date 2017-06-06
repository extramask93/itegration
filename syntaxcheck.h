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
    Result checkLine(QString line,bool ommitUnknown);
    int checkFile(QString filename);
    bool loadJson();
    bool loadDatabase();
    QList<Result> errorlist;
    Result takaFunkcja();
signals:
private:
    QJsonDocument jsonDoc;
    QHash<QString,QString> database;



public slots:
};

#endif // SYNTAXCHECK_H
