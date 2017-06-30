#ifndef BADNAMEEXCEPTION_H
#define BADNAMEEXCEPTION_H
#include<exception>

class BadNameException : public std::exception
{
public:
    BadNameException();
virtual char const * what() const noexcept
    { return "Name should only contain letters [A-Za-z0-9] and have maximum of 8 characters"; }
virtual ~BadNameException() throw() {}
};
#endif // BADNAMEEXCEPTION_H
