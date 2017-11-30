#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>

#include "../Common/toolbox.h"
#include "../Common/throw.h"
#include "../Common/colorShell.h"
#include "Entity.h"
#include "Engine.h"
#include "Yaml.h"

char environment[256] = "";

void *yamlDatabaseCreate(char *);
void *yamlDatabaseDrop(char *);
void *yamlTableLoad(char *, char *, void *);
void *yamlTableCreate(char *, char *, void *);
void *yamlTableInsert(char *, char *, void *);
void *yamlTableSelect(char *, char *, void *, void **, char *, char *, char *, ...);
void *yamlTableUpdate(char *, char *, void *, char *, char *, char *, char *, char *);
void *yamlTableDelete(char *, char *, void *, char *, char *, char *);
void *yamlTableDrop(char *, char *);
Stack *launcher();
void move(Stack *, int);
void push(Stack *, int);
void __realloc(Stack *, int);

void entityGlobalEnv(char *_environment)
{
    sprintf(environment, "%s", _environment);
}

Table tableInit()
{
    Table table;

    table.load = yamlTableLoad;
    table.create = yamlTableCreate;
    table.insert = yamlTableInsert;
    table.select = yamlTableSelect;
    table.update = yamlTableUpdate;
    table.delete = yamlTableDelete;
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

void *yamlDatabaseCreate(char *dbName)
{
    Path path = pathParse(NULL, dbName);
    int status;

    if(_isDatabase(path.dir) == true) {
        return (void *)setError("Exception: a database with this name already exists");
    }

    status = _polyfillMkdir(path.dir);
    if(status == false) {
        return (void *)setError("Exception: unknown error");
    }

    return (void *)setSuccess("Database \"%s\" was created", dbName);
}

void *yamlDatabaseDrop(char *dbName)
{
    Path path = pathParse(NULL, dbName);
    int status;

    if(_isDatabase(path.dir) == false) {
        return (void*)setError("Exception: a database with this name does not exist");
    }

    status = _polyfillRmdir(path.dir);
    if(status == false) {
        return (void *)setError("Exception: unknown error");
    }

    return (void *)setSuccess("Database \"%s\" was deleted", dbName);
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

void *yamlTableLoad(char *dbName, char *tableName, void *_entity)
{
    Entity *entity = (Entity *)_entity;
    Path path = pathParse(tableName, dbName);

    if(_isDatabase(path.dir) == false) {
        return (void *)setError("Exception: a database with this name does not exist");
    }

    if(_isTable(path.path) == false) {
        return (void *)setError("Exception: a table with this name does not exist");
    }

    return entity->_.load(_entity, path.path);
}

void *yamlTableCreate(char *dbName, char *tableName, void *_entity)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;

    if(_isDatabase(path.dir) == false) {
        return (void *)setError("Exception: a database with this name does not exist");
    }

    if(_isTable(path.path) == true) {
        return setError("Exception: a table with this name already exists");
    }

    if(!entity->_.commit(path.path, entity->header, NULL)) {
		return (void *)setError("Exception: unknown error");
	}

    return (void *)setSuccess("Table \"%s\" was created", tableName);
}

void *yamlTableInsert(char *dbName, char *tableName, void *_entity)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;

    Throw *err = (Throw *)entity->_.commit(path.path, NULL, entity->core[entity->length - 1]);
    if(err->err) {
        return (void *)err;
    }

    return (void *)setSuccess("New line was added in the table \"%s\"", tableName);
}

void *yamlTableSelect(char *dbName, char *tableName, void *_entity, void **_stack, char *operator, char *valueA, char *valueB, ...)
{
    Entity *entity = (Entity *)_entity;
    Engine engine = engineInit();
    va_list args;
    Stack *stack = launcher();
    char *choice = NULL;
    int i, j, k;
    int allStatement = 0;

    if((j = contains(valueA, entity->header->data, entity->header->size)) == -1) {
        return (void *)setError("Exception: the \"%s\" column does not exist in this table", valueA);
    }

    va_start(args, valueB);

    while((choice = va_arg(args, char *))) {
        if(allStatement || !strcmp(choice, "*")) {
            allStatement = 1;

            for(k = 0; k < entity->length; k++) {
                for(i = 0; i < entity->core[k]->size; i++) {
                    if(engine.eval(operator, entity->core[k]->data[j], valueB, entity->header->type[j])) {
                        move(stack, k);
                        push(stack, i);
                    }
                }
            }
        } else if((i = contains(choice, entity->header->data, entity->header->size)) == -1) {
            va_end(args);
            destroyStack(stack);
            return (void *)setError("Exception: the \"%s\" column does not exist in this table", choice);
        } else {
            for(k = 0; k < entity->length; k++) {
                if(engine.eval(operator, entity->core[k]->data[j], valueB, entity->header->type[j])) {
                    move(stack, k);
                    push(stack, i);
                }
            }
        }
    }

    *_stack = (void *)stack;
    va_end(args);
    return (void *)setSuccess("");
}

void *yamlTableUpdate(char *dbName, char *tableName, void *_entity, char *valueA, char *valueB, char *operator, char *valueC, char *valueD)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;
    Stack *stack = NULL;
    int i, j;

    if((i = contains(valueA, entity->header->data, entity->header->size)) == -1) {
        return (void *)setError("Exception: the \"%s\" column does not exist in this table", valueA);
    }

    Throw *err = (Throw *)yamlTableSelect(dbName, tableName, entity, (void **)(&stack), operator, valueC, valueD, valueA, NULL);

    if(!err->err) {
        for(i = 0; i < stack->size; i++) {
            if(stack->indexed[i].active == NULL) {
                for(j = 0; j < stack->indexed[i].size; j++) {
                    entity->_.set(entity, i, stack->indexed[i].ids[j], valueB);
                    success("Line #%s was updated in the table \"%s\"\n", entity->core[i]->id, tableName);
                }
            }
        }

        entity->_.reload(entity, path.path);
        destroyStack(stack);
    } else {
        return (void *)err;
    }

    return (void *)setSuccess("");
}

void *yamlTableDelete(char *dbName, char *tableName, void *_entity, char *operator, char *valueA, char *valueB)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;
    Stack *stack;
    int i;

    Throw *err = (Throw *)yamlTableSelect(dbName, tableName, entity, (void **)(&stack), operator, valueA, valueB, "*", NULL);

    if(!err->err) {
        for(i = 0; i < stack->size; i++) {
            if(stack->indexed[i].active == NULL && stack->indexed[i].size) {
                entity->core[i]->status = 'D';
                success("Line #%s was deleted in the table \"%s\"\n", entity->core[i]->id, tableName);

                entity->_.reload(entity, path.path);
                entity->_.remove(entity, i);
            }
        }

        destroyStack(stack);
    } else {
        return (void *)err;
    }

    return (void *)setSuccess("");
}

// debug
Stack *launcher()
{
    Stack *stack = malloc(sizeof(Stack));

    if(stack == NULL) {
        danger(true, "Exception: error with malloc\n");
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
        danger(true, "Exception: error with malloc\n");
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
        danger(true, "Exception: error with malloc\n");
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

void *yamlTableDrop(char *dbName, char *tableName)
{
    Path path = pathParse(tableName, dbName);

    if(_isDatabase(path.dir) == false) {
        return (void *)setError("Exception: a database with this name does not exist");
    }

    if(remove(path.path)) {
        if(errno == ENOENT) {
            return (void *)setError("Exception: a table with this name does not exist");
        } else {
            return (void *)setError("Exception: unknown error");
        }
    }

    return (void *)setSuccess("Table \"%s\" was deleted", tableName);
}
