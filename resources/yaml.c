#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "toolbox.h"
#include "colorShell.h"
#include "yaml.h"

void yamlDatabaseCreate(char *);
void yamlDatabaseDrop(char *);
void yamlTableCreate(char *, char *);
void yamlTableDelete(char *, char *);
// void yamlTableInsert(char *, char *);

Table tableInit()
{
    Table table;

    table.create = yamlTableCreate;
    table.delete = yamlTableDelete;

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

void yamlTableCreate(char *dbName, char *tableName)
{
    Path path = pathParse(tableName, dbName);
    FILE *tableFile = NULL;

    if(_isDatabase(path.dir) == false) {
        danger(false, "Exception: a database with this name does not exist\n");
        return;
    }

    if(_isTable(path.path) == true) {
        danger(false, "Exception: a table with this name already exists\n");
        return;
    }

    tableFile = fopen(path.path, "w");
    if(tableFile) {
        fclose(tableFile);
    }

    success("Table \"%s\" was created\n", tableName);
}

void yamlTableDelete(char *dbName, char *tableName)
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

/* void yamlTableInsert(char *databaseName, char *tableName)
{
    FILE *dbFile = NULL;
    const char *basename = "container";
    char *path = malloc(sizeof(char) * (strlen(basename) + 1 + 4 + 1 + 4 + strlen(EXTNAME) + 1)); // basename + / + database + / + uniqueidentifiant + extname
    char *dbIdentifiant;
    char *tableIdentifiant;

    if(path == NULL) {
        return;
    } else {
        dbIdentifiant = malloc(sizeof(char) * 5);
        tableIdentifiant = malloc(sizeof(char) * 5);

        if(dbIdentifiant == NULL || tableIdentifiant == NULL) {
            _safeFree(&path);
            _safeFree(&dbIdentifiant);
            _safeFree(&tableIdentifiant);

            return;
        } else {
            dbIdentifiant = uniqueIdentifier(databaseName);
            tableIdentifiant = uniqueIdentifier(tableName);

            sprintf(path, "%s/%s", basename, dbIdentifiant);
        }

        if(!_isDatabase(path)) {
            danger(false, "Exception: a database with this name does not exist\n");

            _safeFree(&path);
            _safeFree(&dbIdentifiant);
            _safeFree(&tableIdentifiant);

            return;
        } else {
            sprintf(path, "%s/%s%s", path, tableIdentifiant, EXTNAME);
        }

        if(!_isTable(path)) {
            yamlTableCreate(path);
        }
    }

    success("Table \"%s\" was created\n", tableName);

    _safeFree(&path);
    _safeFree(&dbIdentifiant);
    _safeFree(&tableIdentifiant);
} */
