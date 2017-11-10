#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/toolbox.h"
#include "resources/yaml.h"
#include "resources/colorShell.h"
#include "resources/interface.h"
#include "resources/Entity.h"

int ansiSupport = false;

void displayResult();
void displayFunc(char *, int);
void displayFuncTable(char *, int);

int main(int argc, char **argv)
{
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
        status = interface.prompt(startIndex, displayFunc, &interface.options, "Create a database", "Drop a database", "Add a table", "Insert a line in table", "Update a line in a table", "Delete a line in table", "Drop a table", NULL);
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
    Entity *entity;

    switch(index) {
        case 0: // "Quit"
            success("\nClosing the database shell console...\n");
            return;
        case 1: // "Create a database"
            database = interface.input("database> ");
            yaml.database.create(database);

            break;
        case 2: // "Drop a database"
            database = interface.input("database> ");
            yaml.database.drop(database);

            break;
        case 3: // "Add a table"
            database = interface.input("database> ");
            table = interface.input("table> ");
            // yaml.table.create(database, table);

            /* _interface = interfaceInit();
            _interface.options.quit = true;
            _safeStrdup(&_interface.options.quitLabel, "Save prototype");

            printf("\n");
            welcome("Starting the table shell console...\n\n");

            do {
                status = _interface.prompt(startIndex, displayFuncTable, &_interface.options, "Integer", "Real", "Char", "String", NULL);
                startIndex = false;
            } while(status); */

            entity = EntityInit();

            entity->_.initializer(entity, YAML_REAL, "age");
            entity->_.initializer(entity, YAML_STRING, "username");

            yaml.table.create(database, table, entity);
            freeEntity(entity);

            break;
        case 4: // "Insert a line in table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            entity = EntityInit();

            yaml.table.load(database, table, entity);

            entity->core = entity->_.newStack(entity);
            entity->_.push(entity, "19", NULL);
            entity->_.push(entity, "bricetoutpuissant@gmail.com", NULL);

            yaml.table.insert(database, table, entity);
            freeEntity(entity);

            break;
        case 5: // "Update a line in table"
            break;
        case 6: // "Delete a line in table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            entity = EntityInit();

            yaml.table.load(database, table, entity);

            freeEntity(entity);

            break;
        case 7: // "Drop a table"
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
