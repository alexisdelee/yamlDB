#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

typedef struct
{
    int authorized;
    int lower;
    int equal;
    int upper;
} Algo;

typedef struct
{
    int (*eval)(char *, char *, char *, int);
} Engine;

Engine engineInit();

#endif // ENGINE_H_INCLUDED
