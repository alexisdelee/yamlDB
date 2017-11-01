#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Callback.h"

void createDatabaseFunc(char *);
void dropDatabaseFunc(char *);
void createTableFunc(char *);
void dropTableFunc(char *);
void insertTableFunc(char *);
void updateTableFunc(char *);
void deleteTableFunc(char *);

Table tableInit()
{
    Table table;

    table.create = createTableFunc;
    table.drop = dropTableFunc;
    table.insert = insertTableFunc;
    table.update = updateTableFunc;
    table.delete = deleteTableFunc;

    return table;
}

Database databaseInit()
{
    Database database;

    database.create = createDatabaseFunc;
    database.drop = dropDatabaseFunc;

    return database;
}

Callback callbackInit()
{
    Callback callback;

    callback.database = databaseInit();
    callback.table = tableInit();

    return callback;
}

// callbacks: privates methods

void createDatabaseFunc(char *s)
{

}

void dropDatabaseFunc(char *s)
{

}

void createTableFunc(char *s)
{

}

void dropTableFunc(char *s)
{

}

void insertTableFunc(char *s)
{

}

void updateTableFunc(char *s)
{

}

void deleteTableFunc(char *s)
{

}
