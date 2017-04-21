#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QObject>

class Interpreter : public QObject
{
    Q_OBJECT
public:
    explicit Interpreter(QObject *parent = 0);

signals:
    void robotCommandIssued(QByteArray a);
    void controlCommandIssued(QByteArray a);

public slots:
    void processCommand(QString command);
};

#endif // INTERPRETER_H
