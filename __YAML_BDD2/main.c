#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/dependancies.h"
#include "resources/arguments.h"
#include "resources/Parser.h"
#include "resources/Callback.h"

unsigned char colorModeOption = false;
unsigned char statisticModeOption = false;
char container[256];

int main(int argc, char **argv)
{
	Node *tree = initNode(".", "root");
	Parser parser = parserInit();
	Callback callback = callbackInit();

	analyzerArguments(argc, argv);

	parser.build("CREATE DATABASE @DATABASE", &tree, callback.database.create);
	parser.build("DROP DATABASE @DATABASE", &tree, callback.database.drop);
	parser.build("CREATE TABLE @TABLE @ARGUMENTS", &tree, callback.table.create);
	parser.build("DROP TABLE @TABLE", &tree, callback.table.drop);
	parser.build("INSERT INTO @TABLE VALUES @ARGUMENTS", &tree, callback.table.insert);
	parser.build("UPDATE @TABLE SET @KEY = @VALUE WHERE @KEY @OPERATOR @VALUE", &tree, callback.table.update);
	parser.build("DELETE FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, callback.table.delete);
	// parser.build("SELECT @ARGUMENTS FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	// parser.build("SELECT @ARGUMENTS FROM @TABLE", &tree, func);

	/* ---------- */

	parser.build("create database @DATABASE", &tree, callback.database.create);
	parser.build("drop database @DATABASE", &tree, callback.database.drop);
	parser.build("create table @TABLE @ARGUMENTS", &tree, callback.table.create);
	parser.build("drop table @TABLE", &tree, callback.table.drop);
	parser.build("insert into @TABLE values @ARGUMENTS", &tree, callback.table.insert);
	parser.build("update @TABLE set @KEY = @VALUE where @KEY @OPERATOR @VALUE", &tree, callback.table.update);
	parser.build("delete from @TABLE where @KEY @OPERATOR @VALUE", &tree, callback.table.delete);
	// parser.build("select @ARGUMENTS from @TABLE where @KEY @OPERATOR @VALUE", &tree, func);
	// parser.build("select @ARGUMENTS from @TABLE", &tree, func);

	if(statisticModeOption) {
        parser.statistic(tree, 1);
        printf("\n");
	}

	// parser.sql("create table esgi.test (name VARCHAR, age REAL)", &tree);
	parser.sql("update esgi.test set name = toto where id = ec9f", &tree);

	freeTree(tree);
	// free(tree); /* double free or corruption */

	return 0;
}
