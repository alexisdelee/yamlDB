#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../Common/toolbox.h"
#include "../Common/throw.h"
#include "../Common/colorShell.h"
#include "../Business/arguments.h"
#include "../Database/Yaml.h"
#include "../Database/Entity.h"

typedef struct
{
	Yaml yaml;
	Stack *stack;
	Entity *entity;
} Test;

int ansiSupport = false;

void secureSet(Throw **, Throw *);

int main(int argc, char **argv)
{
    Throw *err = NULL;
	Test test = {yamlInit(), NULL, NULL};
    int activeField = false;

	if(argc >= 2) {
		activeField = true;
	}

	if(argc >= 3) {
        ansiSupport = true;
	}

	// region Database
	if(activeField == false || !strcmp("--database", argv[1])) {
	    printf("Database TU:\n");
		// "Create a database"
		secureSet(&err, test.yaml.database.create("Test"));
		assert(err->err == false);
		welcome("%2s[ok] Create a database\n", "");

		// "Create an other database with the same name"
		secureSet(&err, test.yaml.database.create("Test"));
		assert(err->err == true);
		welcome("%2s[ok] Create an other database with the same name\n", "");

		// "Add a table"
		test.entity = entityInit();

		test.entity->_.initializer(test.entity, YAML_REAL, "age");
		test.entity->_.initializer(test.entity, YAML_STRING, "username");
		test.entity->_.initializer(test.entity, YAML_CHARACTER, "capitalize");

        secureSet(&err, test.yaml.table.create("Test", "test", test.entity));
		assert(err->err == false);
		welcome("%2s[ok] Create a table\n", "");

		// "Add an other table with the same name"
		secureSet(&err, test.yaml.table.create("Test", "test", test.entity));
		assert(err->err == true);
		welcome("%2s[ok] Create an other table with the same name\n", "");

		freeEntity(test.entity);

		// "Insert a line in a table"
		test.entity = entityInit();

		secureSet(&err, test.yaml.table.load("Test", "test", test.entity));
		if(err->err == false) {
			// test.entity->core = test.entity->_.newStack(test.entity);

			test.entity->_.push(test.entity, "666", NULL);
			test.entity->_.push(test.entity, "lucy@gmail.com", NULL);
			test.entity->_.push(test.entity, "L", NULL);

			secureSet(&err, test.yaml.table.insert("Test", "test", test.entity));
			assert(err->err == false && test.entity->length == 1 && test.entity->core[0]->size == 3);
			welcome("%2s[ok] Insert a line in a table\n", "");
		}

		freeEntity(test.entity);

		// "Select a line in a table"
		test.entity = entityInit();

		secureSet(&err, test.yaml.table.load("Test", "test", test.entity));
		if(err->err == false) {
			secureSet(&err, test.yaml.table.select("Test", "test", test.entity, (void **)(&test.stack), "=", "username", "lucy@gmail.com", "*", NULL));
			assert(err->err == false && test.stack->size == 1 && test.stack->indexed[0].active == NULL && test.stack->indexed[0].size == 3);
			welcome("%2s[ok] Select a line with all properties in a table\n", "");

			destroyStack(test.stack);

			secureSet(&err, test.yaml.table.select("Test", "test", test.entity, (void **)(&test.stack), "=", "username", "lucy@gmail.com", "username", "capitalize", NULL));
			assert(err->err == false && test.stack->size == 1 && test.stack->indexed[0].active == NULL && test.stack->indexed[0].size == 2);
			welcome("%2s[ok] Select a line with \"username\" and \"capitalize\" properties in a table\n", "");

			destroyStack(test.stack);
		}

		freeEntity(test.entity);

		// "Update a line in a table"
		test.entity = entityInit();

		secureSet(&err, test.yaml.table.load("Test", "test", test.entity));
		if(err->err == false) {
            secureSet(&err, test.yaml.table.update("Test", "test", test.entity, "username", "satan@gmail.com", ">=", "age", "666"));
			assert(err->err == false && strcmp("satan@gmail.com", test.entity->core[0]->data[1]) == 0);
			welcome("%2s[ok] Update a line in a table\n", "");
		}

		freeEntity(test.entity);

		// "Delete a line in table"
		test.entity = entityInit();

		secureSet(&err, test.yaml.table.load("Test", "test", test.entity));
		if(err->err == false) {
            secureSet(&err, test.yaml.table.delete("Test", "test", test.entity, "=", "username", "satan@gmail.com"));
			assert(err->err == false && test.entity->length == 0);
			welcome("%2s[ok] Delete a line in a table\n", "");
		}

		freeEntity(test.entity);

		// "Drop a table"
		secureSet(&err, test.yaml.table.drop("Test", "test"));
		assert(err->err == false);
		welcome("%2s[ok] Drop a table\n", "");

		// "Drop a database"
		secureSet(&err, test.yaml.database.drop("Test"));
		assert(err->err == false);
		welcome("%2s[ok] Drop a database\n", "");

		secureSet(&err, NULL);
	}
	// endregion

	// region Business
	if(activeField == false || !strcmp("--business", argv[1])) {
		// do something
	}
	// endregion

	// region Services
	if(activeField == false || !strcmp("--services", argv[1])) {
		// do something
	}
	// endregion

    return 0;
}

void secureSet(Throw **err, Throw *result)
{
    if(*err != NULL) {
        free(*err);
    }

    *err = result;
}
