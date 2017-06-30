#ifndef SCRIPT_H
#define SCRIPT_H
#include <QObject>
#include <QFileInfo>
#include <exception>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include "badnameexception.h"

class Script : public QObject
{
    Q_OBJECT
    QString path;
    QStringList content;
public:
    QString getComplaintName() throw();
    Script(QString path_,QObject *parent=nullptr);
    Script(const Script& sc);
    QString getNameWithExtension();
    QString getPath();
    QString getFileName();
    QStringList getContent();
    Script& addCarrets();
    int getNrOfLines();
    QStringList getContentReadyToSend()throw();
    Script& number();
    Script &addLineBreaks();
signals:
private:
    QStringList loadContent() throw();
    Script &addHeading();
public slots:
};

#endif // SCRIPT_H
