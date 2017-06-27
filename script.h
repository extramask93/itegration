#ifndef SCRIPT_H
#define SCRIPT_H
#include <QObject>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

class Script : public QObject
{
    Q_OBJECT
    QString path;
    QStringList content;
public:
    Script(QString path_,QObject *parent=nullptr);
    Script(const Script& sc);
    QString getNameWithExtension();
    QString getPath();
    QString getFileName();
    QStringList getContent();
    Script& addCarrets();
    int getNrOfLines();
    QStringList getContentReadyToSend();
    Script& number();
    Script &addLineBreaks();
signals:
private:
    QStringList loadContent();
    Script &addHeading();
public slots:
};

#endif // SCRIPT_H
