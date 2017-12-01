#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Common/toolbox.h"
#include "../Common/throw.h"
#include "../Database/Yaml.h"
#include "../Database/Entity.h"
#include "Callback.h"

TableStatement tableStatementInit();
DatabaseStatement databaseStatementInit();

void createDatabaseFunc(char *, void *);
void dropDatabaseFunc(char *, void *);
void createTableFunc(char *, void *);
void dropTableFunc(char *, void *);
void insertTableFunc(char *, void *);
void updateTableFunc(char *, void *);
void deleteTableFunc(char *, void *);

TableStatement tableStatementInit()
{
    TableStatement table;

    table.create = createTableFunc;
    table.drop = dropTableFunc;
    table.insert = insertTableFunc;
    table.update = updateTableFunc;
    table.delete = deleteTableFunc;

    return table;
}

DatabaseStatement databaseStatementInit()
{
    DatabaseStatement database;

    database.create = createDatabaseFunc;
    database.drop = dropDatabaseFunc;

    return database;
}

Callback callbackInit()
{
    Callback callback;

    callback.database = databaseStatementInit();
    callback.table = tableStatementInit();

    return callback;
}

// callbacks: privates methods

void createDatabaseFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();

    Throw *err = (Throw *)yaml.database.create(parameters->data[0]);
    printStackError(err, debug);

    if(err != NULL) free(err);
}

void dropDatabaseFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();

    Throw *err = (Throw *)yaml.database.drop(parameters->data[0]);
    printStackError(err, debug);

    if(err != NULL) free(err);
}

void createTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Throw *err;
    int i;

    if((parameters->size - 2) % 2 != 0) {
        err = setError("Exception: create table xxx (int xxx, real xxx, char xxx, varchar xxx)");
        printStackError(err, debug);

        if(err != NULL) free(err);
        return;
    }

    for(i = 2; i < parameters->size; i += 2) {
        if(!strcmp(parameters->data[i], "int") || !strcmp(parameters->data[i], "INT")) {
            entity->_.initializer(entity, YAML_INTEGER, parameters->data[i + 1]);
        } else if(!strcmp(parameters->data[i], "real") || !strcmp(parameters->data[i], "REAL")) {
            entity->_.initializer(entity, YAML_REAL, parameters->data[i + 1]);
        } else if(!strcmp(parameters->data[i], "char") || !strcmp(parameters->data[i], "CHAR")) {
            entity->_.initializer(entity, YAML_CHARACTER, parameters->data[i + 1]);
        } else if(!strcmp(parameters->data[i], "varchar") || !strcmp(parameters->data[i], "VARCHAR")) {
            entity->_.initializer(entity, YAML_STRING, parameters->data[i + 1]);
        } else {
            err = setError("Exception: create table xxx (int xxx, real xxx, char xxx, varchar xxx)");
            printStackError(err, debug);

            break;
        }
    }

    err = (Throw *)yaml.table.create(parameters->data[0], parameters->data[1], entity);
    printStackError(err, debug);

    freeEntity(entity);
    if(err != NULL) free(err);
}

void dropTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();

    Throw *err = (Throw *)yaml.table.drop(parameters->data[0], parameters->data[1]);
    printStackError(err, debug);

    if(err != NULL) free(err);
}

void insertTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Throw *err = NULL;
    int i;

    err = (Throw *)yaml.table.load(parameters->data[0], parameters->data[1], entity);
    if(err->err) {
        printStackError(err, debug);

        freeEntity(entity);
        if(err != NULL) free(err);

        return;
    }

    if(parameters->size > 2) {
        free(err);
        entity->core = entity->_.newStack(entity);

        for(i = 2; i < parameters->size; i++) {
            entity->_.push(entity, parameters->data[i], NULL);
        }

        err = (Throw *)yaml.table.insert(parameters->data[0], parameters->data[1], entity);
    }

    freeEntity(entity);
    if(err != NULL) free(err);
}

void updateTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Throw *err = NULL;

    err = (Throw *)yaml.table.load(parameters->data[0], parameters->data[1], entity);
    if(err->err) {
        printStackError(err, debug);

        freeEntity(entity);
        if(err != NULL) free(err);

        return;
    }

    if(err != NULL) free(err);

    err = (Throw *)yaml.table.update(parameters->data[0], parameters->data[1], entity, parameters->data[2], parameters->data[3], parameters->data[5], parameters->data[4], parameters->data[6]);
    if(err->err) {
        printStackError(err, debug);
    }

    freeEntity(entity);
    if(err != NULL) free(err);
}

void deleteTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Throw *err = NULL;

    err = (Throw *)yaml.table.load(parameters->data[0], parameters->data[1], entity);
    if(err->err) {
        printStackError(err, debug);

        freeEntity(entity);
        if(err != NULL) free(err);

        return;
    }

    if(err != NULL) free(err);

    err = (Throw *)yaml.table.delete(parameters->data[0], parameters->data[1], entity, parameters->data[3], parameters->data[2], parameters->data[4]);
    if(err->err) {
        printStackError(err, debug);
    }

    freeEntity(entity);
    if(err != NULL) free(err);
}
