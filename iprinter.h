#ifndef IPRINTER_H
#define IPRINTER_H
#include <QString>

class IPrinter
{
public:
    IPrinter();
    void virtual printError(QString error) =0;
    void virtual printMessage(QString message) = 0;
};

#endif // IPRINTER_H
