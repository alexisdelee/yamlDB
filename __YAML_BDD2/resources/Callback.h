#ifndef CALLBACKS_H_INCLUDED
#define CALLBACKS_H_INCLUDED

typedef struct Database Database;
struct Database
{
    void (*create)(char *, void *);
    void (*drop)(char *, void *);
};

typedef struct Table Table;
struct Table
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
    Database database;
    Table table;
};

Callback callbackInit();

#endif // CALLBACKS_H_INCLUDED
