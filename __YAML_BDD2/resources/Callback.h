#ifndef CALLBACKS_H_INCLUDED
#define CALLBACKS_H_INCLUDED

typedef struct Database Database;
struct Database
{
    void (*create)(char *);
    void (*drop)(char *);
};

typedef struct Table Table;
struct Table
{
    void (*create)(char *);
    void (*drop)(char *);
    void (*insert)(char *);
    void (*update)(char *);
    void (*delete)(char *);
};

typedef struct Callback Callback;
struct Callback
{
    Database database;
    Table table;
};

Callback callbackInit();

#endif // CALLBACKS_H_INCLUDED
