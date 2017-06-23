#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class Logger : public QObject
{
    Q_OBJECT
    const int maxNrOfHistoryEntries = 255;
    const int maxNrOfLogEntries = 2000;
    QStringList iolist;
    QStringList commandList;
public:
    explicit Logger(QObject *parent = nullptr);
    void log(QString str);
    QStringList loadSession(QString fileName="lastSession.txt");
    void addToSession(QString message);
    QStringList loadCommands(QString fileName="history.txt");



signals:

public slots:
    void saveSession(QString fileName="lastSession.txt");
    void saveCommands(QStringList list,QString fileName="history.txt");
};

#endif // LOGGER_H
