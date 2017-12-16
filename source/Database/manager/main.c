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

void createDatabaseOption();
void dropDatabaseOption();
void addTableOption();
void insertTableOption();
void selectTableOption();
void updateTableOption();
void deleteTableOption();
void dropTableOption();

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
    switch(index) {
        case 0: // "Quit"
            success("\nClosing the database shell console...\n");
            return;
        case 1: // "Create a database"
            createDatabaseOption();
            break;
        case 2: // "Drop a database"
            dropDatabaseOption();
            break;
        case 3: // "Add a table"
            addTableOption();
            break;
        case 4: // "Insert a line in a table"
            insertTableOption();
            break;
        case 5: // "Select lines in a table"
            selectTableOption();
            break;
        case 6: // "Update a line in a table"
            updateTableOption();
            break;
        case 7: // "Delete a line in table"
            deleteTableOption();
            break;
        case 8: // "Drop a table"
            dropTableOption();
            break;
    }
}

void createDatabaseOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();

    char *database = interface.input("database> ");
    Throw *err = (Throw *)yaml.database.create(database);

    if(err->err) {
        printStackError(err, debug);
    } else {
        strcat(err->output, "\n");
        success(err->output);
    }

    _safeFree(&database);
    free(err);
}

void dropDatabaseOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();

    char *database = interface.input("database> ");
    Throw *err = (Throw *)yaml.database.drop(database);

    if(err->err) {
        printStackError(err, debug);
    } else {
        strcat(err->output, "\n");
        success(err->output);
    }

    _safeFree(&database);
    free(err);
}

void addTableOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();
    char *type = NULL;
    char *value = NULL;
    int status = 0;

    char *database = interface.input("database> ");
    char *table = interface.input("table> ");

    Entity *entity = entityInit();

    while(true) {
        welcome("input the type and the data to insert : [1] Integer [2] Real [3] Char [4] Varchar %5s[.send] start creation\n", "");
        type = interface.input("type> ");
        if(!strcmp(type, ".send")) {
            break;
        }

        value = interface.input("data> ");
        if(!strcmp(type, "1")) {
            entity->_.initializer(entity, YAML_INTEGER, value);
            status++;
        } else if(!strcmp(type, "2")) {
            entity->_.initializer(entity, YAML_REAL, value);
            status++;
        } else if(!strcmp(type, "3")) {
            entity->_.initializer(entity, YAML_CHARACTER, value);
            status++;
        } else if(!strcmp(type, "4")) {
            entity->_.initializer(entity, YAML_STRING, value);
            status++;
        } else {
            printStackError(setError("Exception: type unknow"), debug);
        }
    }

    if(status) {
        Throw *err = (Throw *)yaml.table.create(database, table, entity);

        if(err->err) {
            printStackError(err, debug);
        } else {
            strcat(err->output, "\n");
            success(err->output);
        }

        free(err);
    }

    freeEntity(entity);
    _safeFree(&database);
    _safeFree(&table);
    _safeFree(&type);
    _safeFree(&value);
}

void insertTableOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();

    char *database = interface.input("database> ");
    char *table = interface.input("table> ");
    char *value = NULL;
    char header[258];
    int i;

    Entity *entity = entityInit();
    Throw *err = (Throw *)yaml.table.load(database, table, entity);
    if(err->err) {
        printStackError(err, debug);
    } else {
        entity->core = entity->_.newStack(entity);

        welcome("input the value that you want insert in the table\n");
        for(i = 0; i < entity->header->size; i++) {
            _safeFree(&value);

            do {
                sprintf(header, "%s> ", entity->header->data[i]);
                value = interface.input(header);
            } while(entity->_.push(entity, value, NULL) == 0);
        }

        free(err);
        err = yaml.table.insert(database, table, entity);

        if(err->err) {
            printStackError(err, debug);
        } else {
            strcat(err->output, "\n");
            success(err->output);
        }
    }

    freeEntity(entity);
    _safeFree(&database);
    _safeFree(&table);
    _safeFree(&value);
    free(err);
}

void selectTableOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();
    SelectStatement statement;
    Stack *stack = NULL;
    int i, j, id;
    char *isWhereStatement = NULL, *operator = NULL, *key = NULL, *value = NULL;

    char *database = interface.input("database> ");
    char *table = interface.input("table> ");

    Entity *entity = entityInit();
    Throw *err = (Throw *)yaml.table.load(database, table, entity);
    if(err->err) {
        printStackError(err, debug);
    } else {
        statement.size = 0;

        welcome("input the column that you want select in the table %5s[.send] start select\n", "");
        while(true) {
            _safeFree(&value);
            value = interface.input("value> ");

            if(!strcmp(value, ".send")) {
                break;
            } else {
                addStatement(&statement, value);
            }
        }

        _safeFree(&value);
        isWhereStatement = interface.input("use condition ? [y/N]> ");
        if(!strcmp(isWhereStatement, "Y") || !strcmp(isWhereStatement, "y")) {
            key = interface.input("key> ");
            operator = interface.input("operator> ");
            value = interface.input("value> ");

            whereStatement(&statement, operator, key, value);
        } else {
            whereStatement(&statement, "@", "", "");
        }

        err = (Throw *)yaml.table.select(database, table, entity, (void **)(&stack), statement);

        if(err->err) {
            printStackError(err, debug);
        } else {
            /* for(i = 0; i < stack->size; i++) {
                if(stack->indexed[i].active == true) {
                    for(j = 0; j < stack->indexed[i].size; j++) {
                        id = stack->indexed[i].ids[j];
                        if(!j) {
                            sprintf(err->output, "%sid:%s => { ", err->output, entity->core[i]->id);
                        }

                        if(!(entity->header->type[j] & YAML_UNDEFINED)) {
                            sprintf(err->output, "%s\n%2svalue: \"%s\", type: \"%s\"", "", err->output, entity->core[i]->data[id], entity->header->type[id] & YAML_INTEGER ? "int" : (entity->header->type[id] & YAML_REAL ? "real" : (entity->header->type[id] & YAML_CHARACTER ? "char" : "varchar")));
                        }

                        if(j == stack->indexed[i].size - 1) {
                            sprintf(err->output, "%s\n}\n", err->output);
                        }
                    }
                }
            }

            if(err->err) {
                printStackError(err, debug);
            } else {
                strcat(err->output, "\n");
                success(err->output);
            } */

            for(i = 0; i < stack->size; i++) {
                if(stack->indexed[i].active == true) {
                    for(j = 0; j < stack->indexed[i].size; j++) {
                        id = stack->indexed[i].ids[j];
                        if(!j) {
                            sprintf(err->output, "%s<\"%s\">", err->output, entity->core[i]->id);
                        }

                        if(!(entity->header->type[j] & YAML_UNDEFINED)) {
                            if(strcspn(entity->core[i]->data[id], " ") == strlen(entity->core[i]->data[id])) {
                                sprintf(err->output, "%s %s", err->output, entity->core[i]->data[id]);
                            } else {
                                sprintf(err->output, "%s \"%s\"", err->output, entity->core[i]->data[id]);
                            }
                        }

                        if(j == stack->indexed[i].size - 1) {
                            sprintf(err->output, "%s\n", err->output);
                        }
                    }
                }
            }

            if(err->err) {
                printStackError(err, debug);
            } else {
                success(err->output);
            }

            destroyStack(stack);
        }

        freeStatement(&statement);
    }

    freeEntity(entity);
    _safeFree(&database);
    _safeFree(&table);
    _safeFree(&isWhereStatement);
    _safeFree(&operator);
    _safeFree(&key);
    _safeFree(&value);
    free(err);
}

void updateTableOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();

    char *database = interface.input("database> ");
    char *table = interface.input("table> ");
    char *isWhereStatement = NULL, *key = NULL, *value = NULL;
    char *whereKey = NULL, *whereValue = NULL, *operator = NULL;

    Entity *entity = entityInit();
    Throw *err = (Throw *)yaml.table.load(database, table, entity);
    if(err->err) {
        printStackError(err, debug);
    } else {
        free(err);

        key = interface.input("key> ");
        value = interface.input("new value> ");

        isWhereStatement = interface.input("use condition ? [y/N]> ");
        if(!strcmp(isWhereStatement, "Y") || !strcmp(isWhereStatement, "y")) {
            whereKey = interface.input("key> ");
            operator = interface.input("operator> ");
            whereValue = interface.input("value> ");

            err = yaml.table.update(database, table, entity, key, value, operator, whereKey, whereValue);
        } else {
            err = yaml.table.update(database, table, entity, key, value, "@", "", "");
        }

        if(err->err) {
            printStackError(err, debug);
        } else {
            success(err->output);
        }
    }

    freeEntity(entity);
    _safeFree(&database);
    _safeFree(&table);
    _safeFree(&isWhereStatement);
    _safeFree(&key);
    _safeFree(&value);
    _safeFree(&whereKey);
    _safeFree(&whereValue);
    _safeFree(&operator);
    free(err);
}

void deleteTableOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();

    char *database = interface.input("database> ");
    char *table = interface.input("table> ");
    char *isWhereStatement = NULL, *key = NULL, *value = NULL, *operator = NULL;

    Entity *entity = entityInit();
    Throw *err = (Throw *)yaml.table.load(database, table, entity);
    if(err->err) {
        printStackError(err, true);
    } else {
        free(err);

        isWhereStatement = interface.input("use condition ? [y/N]> ");
        if(!strcmp(isWhereStatement, "Y") || !strcmp(isWhereStatement, "y")) {
            key = interface.input("key> ");
            operator = interface.input("operator> ");
            value = interface.input("value> ");

            err = yaml.table.delete(database, table, entity, operator, key, value);
        } else {
            err = yaml.table.delete(database, table, entity, "@", "", "");
        }

        if(err->err) {
            printStackError(err, debug);
        } else {
            strcat(err->output, "\n");
            success(err->output);
        }
    }

    freeEntity(entity);
    _safeFree(&database);
    _safeFree(&table);
    _safeFree(&key);
    _safeFree(&value);
    _safeFree(&operator);
    free(err);
}

void dropTableOption()
{
    Interface interface = interfaceInit();
    Yaml yaml = yamlInit();

    char *database = interface.input("database> ");
    char *table = interface.input("table> ");

    Throw *err = (Throw *)yaml.table.drop(database, table);

    if(err->err) {
        printStackError(err, debug);
    } else {
        strcat(err->output, "\n");
        success(err->output);
    }

    _safeFree(&database);
    _safeFree(&table);
    free(err);
}
