#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Common/toolbox.h"
#include "../../Common/colorShell.h"
#include "../../Common/Interface.h"
#include "../Entity.h"
#include "../Yaml.h"

#include "../../Common/throw.h"
#include "../../Common/settings.h"

int ansiSupport; // color mode
int debug; // debug mode

void displayFunc(char *, int);
void displayFuncTable(char *, int);

int main(int argc, char **argv)
{
    int status = true;
    int startIndex = true;

    Interface interface = interfaceInit();
    interface.options.quit = true;

    Settings settings = getSettings();
    ansiSupport = settings.allowColor;
    debug = settings.debug;

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
        status = interface.prompt(startIndex, displayFunc, &interface.options, "Create a database", "Drop a database", "Add a table", "Insert a line in a table", "Select lines in a table", "Update a line in a table", "Delete a line in table", "Drop a table", NULL);
        startIndex = false;
    } while(status);

    return 0;
}

void displayFunc(char *value, int index)
{
    Interface interface = interfaceInit();
    Interface _interface;
    Yaml yaml = yamlInit();
    Stack *stack = NULL;
    Throw *err = NULL;
    char *database = NULL;
    char *table = NULL;
    int status = true;
    int startIndex = true;
    int i, j, id;
    Entity *entity;

    switch(index) {
        case 0: // "Quit"
            success("\nClosing the database shell console...\n");
            return;
        case 1: // "Create a database"
            database = interface.input("database> ");
            err = (Throw *)yaml.database.create(database);
            printStackError(err, debug);

            break;
        case 2: // "Drop a database"
            database = interface.input("database> ");
            err = (Throw *)yaml.database.drop(database);
            printStackError(err, debug);

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

            entity = entityInit();

            entity->_.initializer(entity, YAML_REAL, "age");
            entity->_.initializer(entity, YAML_STRING, "username");
            entity->_.initializer(entity, YAML_CHARACTER, "capitalize");

            err = (Throw *)yaml.table.create(database, table, entity);
            printStackError(err, debug);
            freeEntity(entity);

            break;
        case 4: // "Insert a line in a table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            entity = entityInit();
            err = (Throw *)yaml.table.load(database, table, entity);
            if(err->err) {
                printStackError(err, debug);
            } else {
                entity->core = entity->_.newStack(entity);

                entity->_.push(entity, "19", NULL);
                entity->_.push(entity, "bricetoutpuissant@gmail.com", NULL);
                entity->_.push(entity, "B", NULL);

                free(err);
                err = yaml.table.insert(database, table, entity);
                printStackError(err, debug);
            }

            freeEntity(entity);

            break;
        case 5: // "Select lines in a table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            entity = entityInit();
            err = (Throw *)yaml.table.load(database, table, entity);
            if(err->err) {
                printStackError(err, debug);
            } else {
                err = (Throw *)yaml.table.select(database, table, entity, (void **)(&stack), "<>", "age", "20", "*", NULL);

                if(err->err) {
                    printStackError(err, debug);
                } else {
                    for(i = 0; i < stack->size; i++) {
                        if(stack->indexed[i].active == NULL) {
                            for(j = 0; j < stack->indexed[i].size; j++) {
                                id = stack->indexed[i].ids[j];
                                if(!j) {
                                    printf("id:%s => { ", entity->core[i]->id);
                                }

                                if(!(entity->header->type[j] & YAML_UNDEFINED)) {
                                    printf("\n%2svalue: \"%s\", type: \"%s\"", "", entity->core[i]->data[id], entity->header->type[id] & YAML_INTEGER ? "integer" : (entity->header->type[id] & YAML_REAL ? "real" : (entity->header->type[id] & YAML_CHARACTER ? "char" : "string")));
                                }

                                if(j == stack->indexed[i].size - 1) {
                                    printf("\n}\n");
                                }
                            }
                        }
                    }

                    destroyStack(stack);
                }
            }

            freeEntity(entity);

            break;
        case 6: // "Update a line in a table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            entity = entityInit();
            err = (Throw *)yaml.table.load(database, table, entity);
            if(err->err) {
                printStackError(err, debug);
            } else {
                free(err);

                err = yaml.table.update(database, table, entity, "username", "toto", ">=", "age", "20");
                if(err->err) {
                    printStackError(err, debug);
                }
            }

            freeEntity(entity);

            break;
        case 7: // "Delete a line in table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            entity = entityInit();
            err = (Throw *)yaml.table.load(database, table, entity);
            if(err->err) {
                printStackError(err, true);
            } else {
                free(err);

                err = yaml.table.delete(database, table, entity, ">=", "age", "20");
                if(err->err) {
                    printStackError(err, debug);
                }
            }

            freeEntity(entity);

            break;
        case 8: // "Drop a table"
            database = interface.input("database> ");
            table = interface.input("table> ");

            err = (Throw *)yaml.table.drop(database, table);
            printStackError(err, debug);

            break;
    }

    _safeFree(&database);
    _safeFree(&table);

    if(err != NULL) free(err);
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
