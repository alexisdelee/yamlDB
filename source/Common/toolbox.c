#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "toolbox.h"
#include "colorShell.h"

char *env();

void _safeFree(char **string)
{
    if(string != NULL) {
        free(*string);
        *string = NULL;
    }
}

void _safeStrdup(char **string, char *content)
{
    _safeFree(string);
    *string = strdup(content);

    if(string == NULL) {
        danger(true, "Exception: error with malloc\n");
    }
}

char* uniqueIdentifier(char *seed)
{
    int checksum = 0;
    int index;
    long size;
    char *id = malloc(sizeof(char) * 5);
    char timestamp[15];

    if(id == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    if(!strcmp(seed, "")) {
        sprintf(timestamp, "%lu", time(NULL));
        size = strlen(timestamp);

        for(index = 0; index < size; index++){
            checksum = (checksum >> 1) + ((checksum & 1) << 15);
            checksum += timestamp[index];
            checksum &= 0xFFFF;
        }

    } else {
        size = strlen(seed);

        for(index = 0; index < size; index++){
            checksum = (checksum >> 1) + ((checksum & 1) << 15);
            checksum += seed[index];
            checksum &= 0xFFFF;
        }
    }

    sprintf(id, "%.4x", checksum);
    return id;
}

int _polyfillMkdir(char *databaseName)
{
    char *command = malloc(sizeof(char) * (strlen(databaseName) + 24));
    int status;

    if(command == NULL) {
        danger(false, "Exception: error with malloc\n");
        return false;
    }

    sprintf(command, "mkdir \"%s\"", databaseName);
    if(OS_Windows) {
        strcat(command, " 2> nul");
    } else {
        strcat(command, " > /dev/null");
    }

    status = system(command);

    _safeFree(&command);
    return !status ? true : false;
}

int _polyfillRmdir(char *databaseName)
{
    char *command = malloc(sizeof(char) * (strlen(databaseName) + 26));
    int status;

    if(command == NULL) {
        danger(false, "Exception: error with malloc\n");
        return false;
    }

    if(OS_Windows) {
        sprintf(command, "rmdir /s /q \"%s\" 2> nul", databaseName);
    } else {
        sprintf(command, "rm -Rf \"%s\" > /dev/null", databaseName);
    }

    status = system(command);

    _safeFree(&command);
    return !status ? true : false;
}

char *env()
{
    char *homedir = NULL;
    static char yamldir[256];

    if((homedir = getenv("~")) == NULL && (homedir = getenv("HOMEPATH")) == NULL) {
        danger(true, "Exception: the environment variable ~ or HOMEPATH is not configure\n");
    }

    sprintf(yamldir, "%s\\.yaml", homedir);

    return yamldir;
}

Path pathParse(char *tableName, char *dbName)
{
    Path path;
    char *tableIdentifiant = NULL;
    char *dbIdentifiant = NULL;

    if(tableName) {
        tableIdentifiant = uniqueIdentifier(tableName);
        sprintf(path.name, "%s", tableIdentifiant);
    } else {
        sprintf(path.name, "%s", "");
    }

    sprintf(path.root, "%s", env());
    strcpy(path.ext, ".yaml");
    sprintf(path.base, "%s%s", path.name, path.ext);

    if(dbName) {
        dbIdentifiant = uniqueIdentifier(dbName);
        sprintf(path.dir, "%s\\%s", path.root, dbIdentifiant);
    } else {
        sprintf(path.dir, "%s", path.root);
    }

    sprintf(path.path, "%s\\%s", path.dir, path.base);

    _safeFree(&tableIdentifiant);
    _safeFree(&dbIdentifiant);

    return path;
}

unsigned int _typeOf(char *data)
{
    double _real;
    unsigned int status = YAML_STRING | YAML_UNDEFINED;

    fflush(stdin);
    if(sscanf(data, "%lf", &_real) == 1) {
        status |= YAML_REAL;
        if((int)_real == _real) {
            status |= YAML_INTEGER;
        }
    }

    if(strlen(data) == 1) {
        status |= YAML_CHARACTER;
    }

    return status;
}

int _isTypeOf(char *data, int type)
{
    unsigned int status = _typeOf(data);

    if(status & type) {
        return true;
    } else {
        return false;
    }
}

int contains(char *value, char **values, int size)
{
    int i;

    for(i = 0; i < size; i++) {
        if(!strcmp(value, values[i])) {
            return i;
        }
    }

    return -1;
}

void freeToken(Token *token)
{
    int i;

    for(i = 0; i < token->size; i++) {
        if(token->data[i]) {
            free(token->data[i]);
        }
    }

    free(token->data);
}
