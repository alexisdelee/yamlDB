#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>

#include "toolbox.h"
#include "colorShell.h"
#include "Entity.h"
#include "Engine.h"
#include "yaml.h"

void yamlDatabaseCreate(char *);
void yamlDatabaseDrop(char *);
void yamlTableLoad(char *, char *, void *);
void yamlTableCreate(char *, char *, void *);
void yamlTableInsert(char *, char *, void *);
void yamlTableDrop(char *, char *);
void yamlTableSelect(char *, char *, void *, void (*callback)(void *, void *), char *, char *, char *, ...);
Stack *launcher();
void move(Stack *, int);
void push(Stack *, int);
void __realloc(Stack *, int);
void destroyStack(Stack *stack);

Table tableInit()
{
    Table table;

    table.load = yamlTableLoad;
    table.create = yamlTableCreate;
    table.insert = yamlTableInsert;
    table.select = yamlTableSelect;
    table.drop = yamlTableDrop;

    return table;
}

Database databaseInit()
{
    Database database;

    database.create = yamlDatabaseCreate;
    database.drop = yamlDatabaseDrop;

    return database;
}

Yaml yamlInit()
{
    Yaml yaml;

    yaml.database = databaseInit();
    yaml.table = tableInit();

    return yaml;
}

// databases: privates methods

int _isDatabase(char *databaseName)
{
    DIR *dbFolder = opendir(databaseName);

    if(dbFolder) {
        closedir(dbFolder);
        return true;
    }

    return false;
}

void yamlDatabaseCreate(char *dbName)
{
    Path path = pathParse(NULL, dbName);
    int status;

    if(_isDatabase(path.dir) == true) {
        danger(false, "Exception: a database with this name already exists\n");
        return;
    }

    status = _polyfillMkdir(path.dir);
    if(status == false) {
        danger(false, "Exception: unknown error\n");
        return;
    }

    success("Database \"%s\" was created\n", dbName);
}

void yamlDatabaseDrop(char *dbName)
{
    Path path = pathParse(NULL, dbName);
    int status;

    if(_isDatabase(path.dir) == false) {
        danger(false, "Exception: a database with this name does not exist\n");
        return;
    }

    status = _polyfillRmdir(path.dir);
    if(status == false) {
        danger(false, "Exception: unknown error\n");
        return;
    }

    success("Database \"%s\" was deleted\n", dbName);
}

// tables: privates methods

int _isTable(char *tableName)
{
    FILE *tableFile = NULL;

    tableFile = fopen(tableName, "r");
    if(tableFile) {
        fclose(tableFile);
        return true;
    }

    return false;
}

void yamlTableLoad(char *dbName, char *tableName, void *_entity)
{
    Entity *entity = (Entity *)_entity;
    Path path = pathParse(tableName, dbName);

    if(_isDatabase(path.dir) == false) {
        danger(false, "Exception: a database with this name does not exist\n");
        return;
    }

    if(_isTable(path.path) == false) {
        danger(false, "Exception: a table with this name does not exist\n");
        return;
    }

    entity->_.load(_entity, path.path);
}

void yamlTableCreate(char *dbName, char *tableName, void *_entity)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;

    if(_isDatabase(path.dir) == false) {
        danger(false, "Exception: a database with this name does not exist\n");
        return;
    }

    if(_isTable(path.path) == true) {
        danger(false, "Exception: a table with this name already exists\n");
        return;
    }

    entity->_.commit(path.path, entity->header, NULL);

    success("Table \"%s\" was created\n", tableName);
}

void yamlTableInsert(char *dbName, char *tableName, void *_entity)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;

    entity->_.commit(path.path, NULL, entity->core[entity->length - 1]);

    success("New line was added in the table \"%s\"\n", tableName);
}

void yamlTableSelect(char *dbName, char *tableName, void *_entity, void (*callback)(void *, void *), char *operator, char *valueA, char *valueB, ...)
{
    Entity *entity = (Entity *)_entity;
    Engine engine = engineInit();
    Stack *stack = launcher();
    va_list args;
    char *choice = NULL;
    int i, j, k;

    for(j = 0; j < entity->header->size; j++) {
        if(!strcmp(valueA, entity->header->data[j])) {
            break;
        } else if(j == entity->header->size - 1) {
            danger(false, "Exception: the \"%s\" column does not exist in this table\n", valueA);
            return;
        }
    }

    va_start(args, valueB);

    while((choice = va_arg(args, char *))) {
        for(i = 0; i < entity->header->size; i++) {
            if(!strcmp(choice, entity->header->data[i])) {
                // debug
                for(k = 0; k < entity->length; k++) {
                    if(engine.eval(operator, entity->core[k]->data[j], valueB, entity->header->type[j])) {
                        move(stack, k);
                        push(stack, i);
                    }
                }
                // debug

                break;
            } else if(i == entity->header->size - 1) {
                danger(false, "Exception: the \"%s\" column does not exist in this table\n", choice);

                va_end(args);
                destroyStack(stack);
                return;
            }
        }
    }

    va_end(args);

    if(callback) {
        callback(entity, stack);
    }

    destroyStack(stack);
}

// debug
Stack *launcher()
{
    Stack *stack = malloc(sizeof(Stack));

    if(stack == NULL) {
        perror("Memory exception\n");
        exit(-1);
    }

    stack->currentPointer = 0;
    stack->size = 0;
    stack->indexed = NULL;

    return stack;
}

void move(Stack *stack, int id)
{
    if(id < 0) {
        return;
    } else if(id >= stack->size) {
        __realloc(stack, id + 1);
    }

    stack->currentPointer = id;
}

void push(Stack *stack, int id)
{
    int _current = stack->currentPointer;

    if(stack->indexed[_current].ids == NULL) {
        stack->indexed[_current].ids = malloc(sizeof(int) * (stack->indexed[_current].size = 1));
    } else {
        stack->indexed[_current].ids = realloc(stack->indexed[_current].ids, sizeof(int) * ++stack->indexed[_current].size);
    }

    if( stack->indexed[_current].ids == NULL) {
        perror("Memory exception\n");
        exit(-1);
    }

    stack->indexed[_current].ids[stack->indexed[_current].size - 1] = id;
}

void __realloc(Stack *stack, int size)
{
    stack->size = size;

    if(stack->indexed == NULL) {
        stack->indexed = malloc(sizeof(Indexed) * stack->size);
    } else if(size >= stack->size) {
        stack->indexed = realloc(stack->indexed, sizeof(Indexed) * stack->size);
    }

    if(stack->indexed == NULL) {
        perror("Memory exception\n");
        exit(-1);
    }

    stack->indexed[stack->size - 1].active = NULL;
    stack->indexed[stack->size - 1].size = 0;
    stack->indexed[stack->size - 1].ids = NULL;
}

void destroyStack(Stack *stack)
{
    int i;

    for(i = 0; i < stack->size; i++) {
        if(stack->indexed[i].active == NULL && stack->indexed[i].ids) {
            free(stack->indexed[i].ids);
        }
    }

    if(stack->indexed) free(stack->indexed);
    if(stack) free(stack);
}
// debug

void yamlTableDrop(char *dbName, char *tableName)
{
    Path path = pathParse(tableName, dbName);

    if(_isDatabase(path.dir) == false) {
        danger(false, "Exception: a database with this name does not exist\n");
        return;
    }

    if(remove(path.path)) {
        if(errno == ENOENT) {
            danger(false, "Exception: a table with this name does not exist\n");
        } else {
            danger(false, "Exception: unknown error\n");
        }

        return;
    }

    success("Table \"%s\" was deleted\n", tableName);
}
