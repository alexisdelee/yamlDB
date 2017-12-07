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
void selectTableFunc(char *, void *);
void selectWhereTableFunc(char *, void *);
void displaySelectResults(Entity *, Stack *);
void informationTableFunc(char *, void *);
void displayInformationResults(Entity *, Stack *, Token *);

TableStatement tableStatementInit()
{
    TableStatement table;

    table.create = createTableFunc;
    table.drop = dropTableFunc;
    table.insert = insertTableFunc;
    table.update = updateTableFunc;
    table.delete = deleteTableFunc;
    table.select = selectTableFunc;
    table.selectWhere = selectWhereTableFunc;
    table.information = informationTableFunc;

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

    free(err);
}

void dropDatabaseFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();

    Throw *err = (Throw *)yaml.database.drop(parameters->data[0]);
    printStackError(err, debug);

    free(err);
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
    free(err);
}

void dropTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();

    Throw *err = (Throw *)yaml.table.drop(parameters->data[0], parameters->data[1]);
    printStackError(err, debug);

    free(err);
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
    free(err);
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

    if(parameters->size == 7) { // where statement
        err = (Throw *)yaml.table.update(parameters->data[0], parameters->data[1], entity, parameters->data[2], parameters->data[3], parameters->data[5], parameters->data[4], parameters->data[6]);
    } else {
        err = (Throw *)yaml.table.update(parameters->data[0], parameters->data[1], entity, parameters->data[2], parameters->data[3], "@", "", "");
    }

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

    if(parameters->size == 5) { // where statement
        err = (Throw *)yaml.table.delete(parameters->data[0], parameters->data[1], entity, parameters->data[3], parameters->data[2], parameters->data[4]);
    } else {
        err = (Throw *)yaml.table.delete(parameters->data[0], parameters->data[1], entity, "@", "", "");
    }

    if(err->err) {
        printStackError(err, debug);
    }

    freeEntity(entity);
    if(err != NULL) free(err);
}

void selectWhereTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Stack *stack = NULL;
    Throw *err = NULL;
    SelectStatement statement;
    int i;

    err = (Throw *)yaml.table.load(parameters->data[parameters->size - 5], parameters->data[parameters->size - 4], entity);
    if(err->err) {
        printStackError(err, debug);
    } else {
        statement.size = 0;

        for(i = 0; i < parameters->size - 5; i++) {
            addStatement(&statement, parameters->data[i]);
        }

        whereStatement(&statement, parameters->data[parameters->size - 2], parameters->data[parameters->size - 3], parameters->data[parameters->size - 1]);

        free(err);
        err = (Throw *)yaml.table.select(parameters->data[parameters->size - 5], parameters->data[parameters->size - 4], entity, (void **)(&stack), statement);
        if(err->err) {
            printStackError(err, debug);
        } else {
            displaySelectResults(entity, stack);
            destroyStack(stack);
        }

        freeStatement(&statement);
    }

    freeEntity(entity);
    free(err);
}

void selectTableFunc(char *s, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Stack *stack = NULL;
    Throw *err = NULL;
    SelectStatement statement;
    int i;

    err = (Throw *)yaml.table.load(parameters->data[parameters->size - 2], parameters->data[parameters->size - 1], entity);
    if(err->err) {
        printStackError(err, debug);
    } else {
        statement.size = 0;

        for(i = 0; i < parameters->size - 2; i++) {
            addStatement(&statement, parameters->data[i]);
        }

        whereStatement(&statement, "@", "", "");

        free(err);
        err = (Throw *)yaml.table.select(parameters->data[parameters->size - 2], parameters->data[parameters->size - 1], entity, (void **)(&stack), statement);
        if(err->err) {
            printStackError(err, debug);
        } else {
            displaySelectResults(entity, stack);
            destroyStack(stack);
        }

        freeStatement(&statement);
    }

    freeEntity(entity);
    free(err);
}

void displaySelectResults(Entity *entity, Stack *stack)
{
    int i, j, id;

    for(i = 0; i < stack->size; i++) {
        if(stack->indexed[i].active == NULL) {
            for(j = 0; j < stack->indexed[i].size; j++) {
                id = stack->indexed[i].ids[j];
                if(!j) {
                    printf("<\"%s\">", entity->core[i]->id);
                }

                if(!(entity->header->type[j] & YAML_UNDEFINED)) {
                    if(strcspn(entity->core[i]->data[id], " ") == strlen(entity->core[i]->data[id])) {
                        printf(" %s", entity->core[i]->data[id]);
                    } else {
                        printf(" \"%s\"", entity->core[i]->data[id]);
                    }
                }

                if(j == stack->indexed[i].size - 1) {
                    printf("\n");
                }
            }
        }
    }
}

void informationTableFunc(char *z, void *_parameters)
{
    Token *parameters = (Token *)_parameters;
    Yaml yaml = yamlInit();
    Entity *entity = entityInit();
    Stack *stack = NULL;
    Throw *err = NULL;
    SelectStatement statement;

    err = (Throw *)yaml.table.load(parameters->data[parameters->size - 2], parameters->data[parameters->size - 1], entity);
    if(err->err) {
        printStackError(err, debug);
    } else {
        statement.size = 0;

        addStatement(&statement, "*");
        whereStatement(&statement, "@", "", "");

        free(err);
        err = (Throw *)yaml.table.select(parameters->data[parameters->size - 2], parameters->data[parameters->size - 1], entity, (void **)(&stack), statement);
        if(err->err) {
            printStackError(err, debug);
        } else {
            displayInformationResults(entity, stack, parameters);
            destroyStack(stack);
        }

        freeStatement(&statement);
    }

    freeEntity(entity);
    free(err);
}

void displayInformationResults(Entity *entity, Stack *stack, Token *parameters)
{
    int i, j, k, id;

    for(i = 0; i < 1; i++) {
        if(stack->indexed[i].active == NULL) {
            for(j = 0; j < stack->indexed[i].size; j++) {
                id = stack->indexed[i].ids[j];

                for(k = 0; k < parameters->size; k++) {
                    if(!strcmp(parameters->data[k], "TABLE_NAME")) {
                        printf("\"%s.%s\" ", parameters->data[parameters->size - 2], parameters->data[parameters->size - 1]);
                    } else if(!strcmp(parameters->data[k], "COLUMN_NAME")) {
                        printf("\"%s\" ", entity->header->data[id]);
                    } else if(!strcmp(parameters->data[k], "DATA_TYPE")) {
                        printf("\"%s\" ", entity->header->type[id] & YAML_INTEGER ? "int" : (entity->header->type[id] & YAML_REAL ? "real" : (entity->header->type[id] & YAML_CHARACTER ? "char" : "varchar")));
                    }
                }

                printf("\n");
            }
        }
    }
}
