#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/toolbox.h"
#include "resources/yaml.h"
#include "resources/interface.h"

int ansiSupport = false;

void displayResult();
void displayFunc(char *, int);

int main(int argc, char **argv)
{
    int status = true;
    int startIndex = true;

    Interface interface = interfaceInit();
    interface.options.quit = true;

    if(argc >= 2 && !strcmp(argv[1], "--color-mode")) {
        ansiSupport = true;
    }

    do {
        status = interface.prompt(startIndex, displayFunc, &interface.options, "Create a database", "Add a table", "Delete a table", "Drop a database", NULL);
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
    Yaml yaml = yamlInit();
    char *database = NULL;
    char *table = NULL;

    switch(index) {
        case 1:
            database = interface.input("database> ");
            yaml.database.create(database);

            break;
        case 2:
            database = interface.input("database> ");
            table = interface.input("table> ");
            yaml.table.create(database, table);

            break;
        case 3:
            database = interface.input("database> ");
            table = interface.input("table> ");
            yaml.table.delete(database, table);

            break;
        case 4:
            database = interface.input("database> ");
            yaml.database.drop(database);

            break;
    }

    _safeFree(&database);
    _safeFree(&table);
}
