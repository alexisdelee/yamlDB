#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "toolbox.h"
#include "colorShell.h"
#include "Entity.h"
#include "yaml.h"

void yamlDatabaseCreate(char *);
void yamlDatabaseDrop(char *);
void yamlTableLoad(char *, char *, void *);
void yamlTableCreate(char *, char *, void *);
void yamlTableInsert(char *, char *, void *);
void yamlTableDrop(char *, char *);

Table tableInit()
{
    Table table;

    table.load = yamlTableLoad;
    table.create = yamlTableCreate;
    table.insert = yamlTableInsert;
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
    Path path = pathParse(tableName, dbName);

    if(_isDatabase(path.dir) == false) {
        danger(false, "Exception: a database with this name does not exist\n");
        return;
    }

    if(_isTable(path.path) == false) {
        danger(false, "Exception: a table with this name does not exist\n");
        return;
    }

    loadYaml(_entity, path.path);
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
