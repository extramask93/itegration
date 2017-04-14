#include "result.h"

Result::Result()
{
    Result(0,0,"");
}
Result::Result(unsigned int code, unsigned int line, QString string): errorCode{code}, lineNr{line}, errorString{string}{}
