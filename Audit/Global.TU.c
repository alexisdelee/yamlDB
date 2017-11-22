#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../Common/toolbox.h"
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

int main(int argc, char **argv)
{
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
		assert(test.yaml.database.create("Test") == true);
		welcome("%2s[ok] Create a database\n", "");

		// "Create an other database with the same name"
		assert(test.yaml.database.create("Test") == false);
		welcome("%2s[ok] Create an other database with the same name\n", "");

		// "Add a table"
		test.entity = entityInit();

		test.entity->_.initializer(test.entity, YAML_REAL, "age");
		test.entity->_.initializer(test.entity, YAML_STRING, "username");
		test.entity->_.initializer(test.entity, YAML_CHARACTER, "capitalize");

		assert(test.yaml.table.create("Test", "test", test.entity) == true);
		welcome("%2s[ok] Create a table\n", "");

		// "Add an other table with the same name"
		assert(test.yaml.table.create("Test", "test", test.entity) == false);
		welcome("%2s[ok] Create an other table with the same name\n", "");

		freeEntity(test.entity);

		// "Insert a line in a table"
		test.entity = entityInit();

		if(test.yaml.table.load("Test", "test", test.entity)) {
			// test.entity->core = test.entity->_.newStack(test.entity);

			test.entity->_.push(test.entity, "666", NULL);
			test.entity->_.push(test.entity, "lucy@gmail.com", NULL);
			test.entity->_.push(test.entity, "L", NULL);

			test.yaml.table.insert("Test", "test", test.entity);

			assert(test.entity->length == 1 && test.entity->core[0]->size == 3);
			welcome("%2s[ok] Insert a line in a table\n", "");
		}

		freeEntity(test.entity);

		// "Select a line in a table"
		test.entity = entityInit();

		if(test.yaml.table.load("Test", "test", test.entity)) {
			test.stack = test.yaml.table.select("Test", "test", test.entity, "=", "username", "lucy@gmail.com", "*", NULL);
			assert(test.stack);

			assert(test.stack->size == 1 && test.stack->indexed[0].active == NULL && test.stack->indexed[0].size == 3);
			welcome("%2s[ok] Select a line with all properties in a table\n", "");

			destroyStack(test.stack);

			test.stack = test.yaml.table.select("Test", "test", test.entity, "=", "username", "lucy@gmail.com", "username", "capitalize", NULL);
			assert(test.stack);

			assert(test.stack->size == 1 && test.stack->indexed[0].active == NULL && test.stack->indexed[0].size == 2);
			welcome("%2s[ok] Select a line with \"username\" and \"capitalize\" properties in a table\n", "");

			destroyStack(test.stack);
		}

		freeEntity(test.entity);

		// "Update a line in a table"
		test.entity = entityInit();

		if(test.yaml.table.load("Test", "test", test.entity)) {
			assert(test.yaml.table.update("Test", "test", test.entity, "username", "satan@gmail.com", ">=", "age", "666") == true);
			assert(strcmp("satan@gmail.com", test.entity->core[0]->data[1]) == 0);
			welcome("%2s[ok] Update a line in a table\n", "");
		}

		freeEntity(test.entity);

		// "Delete a line in table"
		test.entity = entityInit();

		if(test.yaml.table.load("Test", "test", test.entity)) {
			assert(test.yaml.table.delete("Test", "test", test.entity, "=", "username", "satan@gmail.com") == true);
			assert(test.entity->length == 0);
			welcome("%2s[ok] Delete a line in a table\n", "");
		}

		freeEntity(test.entity);

		// "Drop a table"
		assert(test.yaml.table.drop("Test", "test") == true);
		welcome("%2s[ok] Drop a table\n", "");

		// "Drop a database"
		assert(test.yaml.database.drop("Test") == true);
		welcome("%2s[ok] Drop a database\n", "");
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
