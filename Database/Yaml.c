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
void *yamlTableSelect(char *, char *, void *, void **, SelectStatement);
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

void *yamlTableSelect(char *dbName, char *tableName, void *_entity, void **_stack, SelectStatement statement)
{
    Entity *entity = (Entity *)_entity;
    Engine engine = engineInit();
    Stack *stack = launcher();
    int i, j, k;
    int selectedId;
    int allStatement = 0;

    if((selectedId = contains(statement.where.key, entity->header->data, entity->header->size)) == -1) {
        if(strcmp(statement.where.operator, "@")) {
            return (void *)setError("Exception: the \"%s\" column does not exist in this table", statement.where.key);
        }
    }

    for(i = 0; i < statement.size; i++) {
        if(allStatement || !strcmp(statement.keys[i], "*")) {
            allStatement = true;

            for(j = 0; j < entity->length; j++) {
                for(k = 0; k < entity->core[j]->size; k++) {
                    if(!strcmp(statement.where.operator, "@")
                       || engine.eval(statement.where.operator, entity->core[j]->data[selectedId], statement.where.value, entity->header->type[selectedId])) {
                        move(stack, j);
                        push(stack, k);
                    }
                }
            }
        } else if((k = contains(statement.keys[i], entity->header->data, entity->header->size)) == -1) {
            destroyStack(stack);
            return (void *)setError("Exception: the \"%s\" column does not exist in this table", statement.keys[i]);
        } else {
            for(j = 0; j < entity->length; j++) {
                if(!strcmp(statement.where.operator, "@")
                   || engine.eval(statement.where.operator, entity->core[j]->data[selectedId], statement.where.value, entity->header->type[selectedId])) {
                    move(stack, j);
                    push(stack, k);
                }
            }
        }
    }

    *_stack = (void *)stack;
    return (void *)setSuccess("");
}

void *yamlTableUpdate(char *dbName, char *tableName, void *_entity, char *valueA, char *valueB, char *operator, char *valueC, char *valueD)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;
    Stack *stack = NULL;
    SelectStatement statement;
    int i, j;

    if((i = contains(valueA, entity->header->data, entity->header->size)) == -1) {
        return (void *)setError("Exception: the \"%s\" column does not exist in this table", valueA);
    }

    statement.size = 0;
    addStatement(&statement, valueA);
    whereStatement(&statement, operator, valueC, valueD);
    Throw *err = (Throw *)yamlTableSelect(dbName, tableName, entity, (void **)(&stack), statement);

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
        freeStatement(&statement);
        return (void *)err;
    }

    freeStatement(&statement);
    return (void *)setSuccess("");
}

void *yamlTableDelete(char *dbName, char *tableName, void *_entity, char *operator, char *valueA, char *valueB)
{
    Path path = pathParse(tableName, dbName);
    Entity *entity = (Entity *)_entity;
    Stack *stack;
    SelectStatement statement;
    int i;

    statement.size = 0;
    addStatement(&statement, "*");
    whereStatement(&statement, operator, valueA, valueB);
    Throw *err = (Throw *)yamlTableSelect(dbName, tableName, entity, (void **)(&stack), statement);

    if(!err->err) {
        for(i = 0; i < stack->size; i++) {
            if(stack->indexed[i].active == NULL && stack->indexed[i].size) {
                entity->core[i]->status = 'D';
                success("Line #%s was deleted in the table \"%s\"\n", entity->core[i]->id, tableName);
            }
        }

        entity->_.reload(entity, path.path);
        destroyStack(stack);
    } else {
        freeStatement(&statement);
        return (void *)err;
    }

    freeStatement(&statement);
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

void addStatement(SelectStatement *statement, char *value)
{
    if(statement->size == 0) {
        statement->keys = malloc(sizeof(char *) * ++statement->size);
    } else {
        statement->keys = realloc(statement->keys, sizeof(char *) * ++statement->size);
    }

    if(statement->keys == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    statement->keys[statement->size - 1] = strdup(value);
    if(statement->keys[statement->size - 1] == NULL) {
        danger(true, "Exception: error with malloc\n");
    }
}

void whereStatement(SelectStatement *statement, char *operator, char *key, char *value)
{
    statement->where.operator = strdup(operator);
    if(statement->where.operator == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    if(key) {
        statement->where.key = strdup(key);
        if(statement->where.key == NULL) {
            danger(true, "Exception: error with malloc\n");
        }
    } else {
        statement->where.key = NULL;
    }

    if(value) {
        statement->where.value = strdup(value);
        if(statement->where.value == NULL) {
            danger(true, "Exception: error with malloc\n");
        }
    } else {
        statement->where.value = NULL;
    }
}

void freeStatement(SelectStatement *statement)
{
    int i;

    for(i = 0; i < statement->size; i++) {
        if(statement->keys[i] != NULL) {
            free(statement->keys[i]);
        }
    }

    if(statement->keys != NULL) free(statement->keys);
    if(statement->where.key != NULL) free(statement->where.key);
    if(statement->where.value != NULL) free(statement->where.value);
    if(statement->where.operator != NULL) free(statement->where.operator);
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
