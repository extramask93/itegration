#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QObject>
#include "syntaxcheck.h"
#include "result.h"
#include "interpreter.h"
#include "console.h"
#include "iprinter.h"

class Interpreter : public QObject
{
    Q_OBJECT
private:
    SyntaxCheck checker;
    IPrinter *console;
    bool checking;
public:
    explicit Interpreter(IPrinter *console,QObject *parent = 0);
    bool isCheckingOn();
    void setCheckingOn(bool);

signals:
    void changed();
    void robotCommandIssued(QByteArray a);
    void controlCommandIssued(QString a);

public slots:
    void processCommand(QString command);
    int processScript(QString fileName);
    void toggleChecker();
};

#endif // INTERPRETER_H
