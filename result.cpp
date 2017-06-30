#include "result.h"

Result::Result()
{
    Result(0,0,"");
}
Result::Result(bool code, unsigned int line, QString string): isError{code}, lineNr{line}, errorString{string}{}
