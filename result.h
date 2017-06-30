#ifndef RESULT_H
#define RESULT_H
#include <QString>

class Result
{
public:
    Result();
    Result(bool, unsigned int, QString);
    bool isError;
    unsigned int lineNr;
    QString errorString;
};

#endif // RESULT_H
