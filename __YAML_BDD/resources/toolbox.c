#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "toolbox.h"
#include "colorShell.h"

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

    sprintf(path.root, "%s", ROOT);
    sprintf(path.ext, "%s", EXTNAME);
    sprintf(path.base, "%s%s", path.name, path.ext);

    if(dbName) {
        dbIdentifiant = uniqueIdentifier(dbName);
        sprintf(path.dir, "%s/%s", path.root, dbIdentifiant);
    } else {
        sprintf(path.dir, "%s", path.root);
    }

    sprintf(path.path, "%s/%s", path.dir, path.base);

    _safeFree(&tableIdentifiant);
    _safeFree(&dbIdentifiant);

    return path;
}
