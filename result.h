#ifndef RESULT_H
#define RESULT_H
#include <QString>

class Result
{
public:
    Result();
    Result(unsigned int, unsigned int, QString);
    unsigned int errorCode;
    unsigned int lineNr;
    QString errorString;
};

#endif // RESULT_H
