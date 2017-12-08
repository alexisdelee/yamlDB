#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../Common/toolbox.h"
#include "../Common/throw.h"
#include "../Common/colorShell.h"
#include "../Database/Yaml.h"
#include "../Database/Entity.h"

typedef struct
{
	Yaml yaml;
	Stack *stack;
	Entity *entity;
	Throw *err;
} Test;

int ansiSupport;

void secureSet(Throw **, Throw *);

int main(int argc, char **argv)
{
    SelectStatement statement;
	Test test = {yamlInit(), NULL, NULL, NULL};

	ansiSupport = false; // no color mode to execute tests

    // "Create a database"
    secureSet(&test.err, test.yaml.database.create("Test"));
    assert(test.err->err == false);
    welcome("%2s[ok] Create a database\n", "");

    // "Create an other database with the same name"
    secureSet(&test.err, test.yaml.database.create("Test"));
    assert(test.err->err == true);
    welcome("%2s[ok] Create an other database with the same name\n", "");

    // "Add a table"
    test.entity = entityInit();

    test.entity->_.initializer(test.entity, YAML_REAL, "age");
    test.entity->_.initializer(test.entity, YAML_STRING, "username");
    test.entity->_.initializer(test.entity, YAML_CHARACTER, "capitalize");

    secureSet(&test.err, test.yaml.table.create("Test", "test", test.entity));
    assert(test.err->err == false);
    welcome("%2s[ok] Create a table\n", "");

    // "Add an other table with the same name"
    secureSet(&test.err, test.yaml.table.create("Test", "test", test.entity));
    assert(test.err->err == true);
    welcome("%2s[ok] Create an other table with the same name\n", "");

    freeEntity(test.entity);

    // "Insert a line in a table"
    test.entity = entityInit();

    secureSet(&test.err, test.yaml.table.load("Test", "test", test.entity));
    if(test.err->err == false) {
        // test.entity->core = test.entity->_.newStack(test.entity);

        test.entity->_.push(test.entity, "666", NULL);
        test.entity->_.push(test.entity, "lucy@gmail.com", NULL);
        test.entity->_.push(test.entity, "L", NULL);

        secureSet(&test.err, test.yaml.table.insert("Test", "test", test.entity));
        assert(test.err->err == false && test.entity->length == 1 && test.entity->core[0]->size == 3);
        welcome("%2s[ok] Insert a line in a table\n", "");
    }

    freeEntity(test.entity);

    // "Select a line in a table"
    test.entity = entityInit();

    secureSet(&test.err, test.yaml.table.load("Test", "test", test.entity));
    if(test.err->err == false) {
        // secureSet(&test.err, test.yaml.table.select("Test", "test", test.entity, (void **)(&test.stack), "=", "username", "lucy@gmail.com", "*", NULL));
        statement.size = 0;
        addStatement(&statement, "*");
        whereStatement(&statement, "=", "username", "lucy@gmail.com");

        secureSet(&test.err, test.yaml.table.select("Test", "test", test.entity, (void **)(&test.stack), statement));
        assert(test.err->err == false && test.stack->size == 1 && test.stack->indexed[0].active == NULL && test.stack->indexed[0].size == 3);
        welcome("%2s[ok] Select a line with all properties in a table\n", "");

        destroyStack(test.stack);
        freeStatement(&statement);

        statement.size = 0;
        addStatement(&statement, "username");
        addStatement(&statement, "capitalize");
        whereStatement(&statement, "=", "username", "lucy@gmail.com");

        secureSet(&test.err, test.yaml.table.select("Test", "test", test.entity, (void **)(&test.stack), statement));
        assert(test.err->err == false && test.stack->size == 1 && test.stack->indexed[0].active == NULL && test.stack->indexed[0].size == 2);
        welcome("%2s[ok] Select a line with \"username\" and \"capitalize\" properties in a table\n", "");

        destroyStack(test.stack);
        freeStatement(&statement);
    }

    freeEntity(test.entity);

    // "Update a line in a table"
    test.entity = entityInit();

    secureSet(&test.err, test.yaml.table.load("Test", "test", test.entity));
    if(test.err->err == false) {
        secureSet(&test.err, test.yaml.table.update("Test", "test", test.entity, "username", "satan@gmail.com", "=", "age", "666"));
        assert(test.err->err == false && strcmp("satan@gmail.com", test.entity->core[0]->data[1]) == 0);
        welcome("%2s[ok] Update a line in a table\n", "");
    }

    freeEntity(test.entity);

    // "Delete a line in table"
    test.entity = entityInit();

    secureSet(&test.err, test.yaml.table.load("Test", "test", test.entity));
    if(test.err->err == false) {
        secureSet(&test.err, test.yaml.table.delete("Test", "test", test.entity, "=", "username", "satan@gmail.com"));
        assert(test.err->err == false);

        statement.size = 0;
        addStatement(&statement, "*");
        whereStatement(&statement, "@", "", "");

        secureSet(&test.err, test.yaml.table.select("Test", "test", test.entity, (void **)(&test.stack), statement));
        assert(test.err->err == false && test.stack->size == 1 && test.stack->indexed[0].active == false);
        welcome("%2s[ok] Delete a line in a table\n", "");

        destroyStack(test.stack);
        freeStatement(&statement);
    }

    freeEntity(test.entity);

    // "Drop a table"
    secureSet(&test.err, test.yaml.table.drop("Test", "test"));
    assert(test.err->err == false);
    welcome("%2s[ok] Drop a table\n", "");

    // "Drop a database"
    secureSet(&test.err, test.yaml.database.drop("Test"));
    assert(test.err->err == false);
    welcome("%2s[ok] Drop a database\n", "");

    secureSet(&test.err, NULL);
    success("\n%15sAll the tests are completed\n", "");

    return 0;
}

void secureSet(Throw **err, Throw *result)
{
    if(*err != NULL) {
        free(*err);
    }

    *err = result;
}
