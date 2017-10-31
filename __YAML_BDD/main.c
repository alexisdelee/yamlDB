#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/toolbox.h"
#include "resources/yaml.h"
#include "resources/colorShell.h"
#include "resources/interface.h"

int ansiSupport = false;

void displayResult();
void displayFunc(char *, int);
void displayFuncTable(char *, int);

typedef struct Data Data;
struct Data
{
    char key[256];
    void *value;
    int dataType;
};

int yamlTableInsert(int size, Data *data, char *tableName)
{
    FILE *tableFile = NULL;
    char type[520];
    int i;

    tableFile = fopen(tableName, "a");
    if(tableFile == NULL) {
        return false;
    }

    for(i = 0; i < size; i++) {
        switch(data[i].dataType) {
            case 1:
                sprintf(type, "%s: [%d, \"%d\"]", data[i].key, data[i].dataType, *((int *)data[i].value));
                break;
            case 2:
                sprintf(type, "%s: [%d, \"%lf\"]", data[i].key, data[i].dataType, *((double *)data[i].value));
                break;
            case 4:
                sprintf(type, "%s: [%d, \"%c\"]", data[i].key, data[i].dataType, *((char *)data[i].value));
                break;
            case 8:
                sprintf(type, "%s: [%d, \"%s\"]", data[i].key, data[i].dataType, *((char **)data[i].value));
                break;
            default:
                return false;
        }

        fprintf(tableFile, "%s\n", type);
    }

    return true;
}

int main(int argc, char **argv)
{
    // debug
    /* const char *a = "name: [4, \"toto\"]";
    char key[256];
    char value[256];
    int dataType;

    int s = sscanf(a, "%[^:]: [%d, \"%[^\"]]", key, &dataType, value);

    if(s == 3) {
        printf("[%s][%s][%d]", key, value, dataType);
    }

    Data *data = malloc(sizeof(Data));
    sprintf(data.key, "%d\n", key); */

    /* int size = 3;
    Path path = pathParse("table", "database");
    Data *data = malloc(sizeof(Data) * size);

    double b = 3.14;
    char *c = "toto";
    char d = 'T';

    sprintf(data[0].key, "%s", "average");
    data[0].value = &b;
    data[0].dataType = 2;

    sprintf(data[1].key, "%s", "name");
    data[1].value = &c;
    data[1].dataType = 8;

    sprintf(data[2].key, "%s", "initialLetter");
    data[2].value = &d;
    data[2].dataType = 4;

    int _status = yamlTableInsert(size, data, path.path);
    printf("status: %d [%s]\n", _status, path.path);
    free(data);

    return 0; */
    // debug

    int status = true;
    int startIndex = true;

    Interface interface = interfaceInit();
    interface.options.quit = true;

    if(argc >= 2 && !strcmp(argv[1], "--color-mode")) {
        ansiSupport = true;
    }

    welcome("Starting the database shell console...\n\n");

    printf(" /$$     /$$ /$$$$$$  /$$      /$$ /$$             /$$$$$$$  /$$$$$$$ \n");
    printf("|  $$   /$$//$$__  $$| $$$    /$$$| $$            | $$__  $$| $$__  $$ \n");
    printf(" \\  $$ /$$/| $$  \\ $$| $$$$  /$$$$| $$            | $$  \\ $$| $$  \\ $$ \n");
    printf("  \\  $$$$/ | $$$$$$$$| $$ $$/$$ $$| $$            | $$  | $$| $$$$$$$ \n");
    printf("   \\  $$/  | $$__  $$| $$  $$$| $$| $$            | $$  | $$| $$__  $$ \n");
    printf("    | $$   | $$  | $$| $$\\  $ | $$| $$            | $$  | $$| $$  \\ $$ \n");
    printf("    | $$   | $$  | $$| $$ \\/  | $$| $$$$$$$$      | $$$$$$$/| $$$$$$$/ \n");
    printf("    |__/   |__/  |__/|__/     |__/|________/      |_______/ |_______/ \n\n");

    do {
        status = interface.prompt(startIndex, displayFunc, &interface.options, "Create a database", "Drop a database", "Add a table", "Insert a line in table", "Delete a line in table", "Drop a table", NULL);
        startIndex = false;
    } while(status);

    return 0;
}

void displayResult()
{
    char data[3][3][50] = {
        { "@id", "Key", "Value" },
        { "eb8a", "_hello", "Hello world" },
        { "f0dc", "_holla", "Holla ?" }
    };

    int maxLengthCoord[3] = { 1, 1, 1 };

    int sizeRows = 3, sizeColumns = 3;
    int i, j;

    printf(" ------ -------- -------------\n");

    for(i = 0; i < sizeRows; i++) {
        for(j = 0; j < sizeColumns; j++) {
            if(!j) {
                printf("| ");
            }

            printf("%*s | ", strlen(data[maxLengthCoord[j]][j]), data[i][j]);
        }

        printf("\n ------ -------- -------------\n");

        if(!i) {
            printf(" ------ -------- -------------\n");
        }
    }
}

void displayFunc(char *value, int index)
{
    Interface interface = interfaceInit();
    Interface _interface;
    Yaml yaml = yamlInit();
    char *database = NULL;
    char *table = NULL;
    int status = true;
    int startIndex = true;

    switch(index) {
        case 0:
            success("\nClosing the database shell console...\n");
            return;
        case 1:
            database = interface.input("database> ");
            yaml.database.create(database);

            break;
        case 2:
            database = interface.input("database> ");
            yaml.database.drop(database);

            break;
        case 3:
            /* database = interface.input("database> ");
            table = interface.input("table> ");
            yaml.table.create(database, table); */

            _interface = interfaceInit();
            _interface.options.quit = true;
            _safeStrdup(&_interface.options.quitLabel, "Save prototype");

            printf("\n");
            welcome("Starting the table shell console...\n\n");

            do {
                status = _interface.prompt(startIndex, displayFuncTable, &_interface.options, "Integer", "Real", "Char", "String", NULL);
                startIndex = false;
            } while(status);

            break;
        case 4:
        case 5:
            break;
        case 6:
            database = interface.input("database> ");
            table = interface.input("table> ");
            yaml.table.drop(database, table);

            break;
    }

    _safeFree(&database);
    _safeFree(&table);
}

void displayFuncTable(char *value, int index)
{
    switch(index) {
        case 0:
            success("\nClosing the table shell console...\n\n");
            return;
        default:
            printf("[%s] %d\n", value, index);
    }
}
