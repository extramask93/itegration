#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QObject>
#include "syntaxcheck.h"
#include "result.h"
#include "interpreter.h"
#include "console.h"
#include "serialport.h"
#include "iprinter.h"

class Interpreter : public QObject
{
    Q_OBJECT
private:
    SyntaxCheck checker;
    IPrinter *console;
    SerialPort *serial;
    bool checking;
public:
    explicit Interpreter(IPrinter *console,SerialPort *serial_,QObject *parent = 0);
    ~Interpreter();
    bool isCheckingOn();
    void setCheckingOn(bool);

signals:
    void changed();
    void robotCommandIssued(QByteArray a);
    void controlCommandIssued(QString a);
    void errorOccured(QString error);

public slots:
    void processCommand(QString command);
    int processScript(Script script);
    void setOmmiting(bool state);
    void toggleChecker();
};

#endif // INTERPRETER_H
