#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dependancies.h"
#include "Callback.h"

void createDatabaseFunc(char *, void *);
void dropDatabaseFunc(char *, void *);
void createTableFunc(char *, void *);
void dropTableFunc(char *, void *);
void insertTableFunc(char *, void *);
void updateTableFunc(char *, void *);
void deleteTableFunc(char *, void *);

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

void createDatabaseFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    // do something
}

void dropDatabaseFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    // do something
}

void createTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    int i;

	printf("[%s]\n", s);
	for(i = 0; i < parameters->size; i++) {
		printf("[%s]\n", parameters->data[i]);
	}
}

void dropTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    // do something
}

void insertTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    // do something
}

void updateTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    // do something
}

void deleteTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    // do something
}
