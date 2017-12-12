#ifndef THROW_H_INCLUDED
#define THROW_H_INCLUDED

#define setSuccess(...) _setSuccess(__FILE__, __LINE__, __VA_ARGS__)
#define setError(...) _setError(__FILE__, __LINE__, __VA_ARGS__)

typedef struct
{
    int err;
    char message[256];
    char file[256];
    int line;
    char output[10001];
} Throw;

Throw *_setError(char *, int, char *, ...);
Throw *_setSuccess(char *, int, char *, ...);
void printStackError(Throw *, int);

#endif // THROW_H_INCLUDED
