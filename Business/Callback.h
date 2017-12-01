#ifndef CALLBACKS_H_INCLUDED
#define CALLBACKS_H_INCLUDED

extern int debug;

typedef struct DatabaseStatement DatabaseStatement;
struct DatabaseStatement
{
    void (*create)(char *, void *);
    void (*drop)(char *, void *);
};

typedef struct TableStatement TableStatement;
struct TableStatement
{
    void (*create)(char *, void *);
    void (*drop)(char *, void *);
    void (*insert)(char *, void *);
    void (*update)(char *, void *);
    void (*delete)(char *, void *);
};

typedef struct Callback Callback;
struct Callback
{
    DatabaseStatement database;
    TableStatement table;
};

Callback callbackInit();

#endif // CALLBACKS_H_INCLUDED
