#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/Parser.h"

void func(char *);

int main(int argc, char **argv)
{
	Node *tree = initNode(".", "root");
	Parser parser = parserInit();

	parser.build("CREATE DATABASE @DATABASE", &tree, func);
	parser.build("DROP DATABASE @DATABASE", &tree, func);
	parser.build("CREATE TABLE @TABLE @ARGUMENTS", &tree, func);
	parser.build("DROP TABLE @TABLE", &tree, func);
	parser.build("INSERT INTO @TABLE VALUES @ARGUMENTS", &tree, func);
	parser.build("UPDATE @TABLE SET @KEY = @VALUE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	parser.build("DELETE FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	parser.build("SELECT @ARGUMENTS FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	parser.build("SELECT @ARGUMENTS FROM @TABLE", &tree, func);

	/* ---------- */

	parser.build("create database @DATABASE", &tree, func);
	parser.build("drop database @DATABASE", &tree, func);
	parser.build("create table @TABLE @ARGUMENTS", &tree, func);
	parser.build("drop table @TABLE", &tree, func);
	parser.build("insert into @TABLE values @ARGUMENTS", &tree, func);
	parser.build("update @TABLE set @KEY = @VALUE where @KEY @OPERATOR @VALUE", &tree, func);
	parser.build("delete from @TABLE where @KEY @OPERATOR @VALUE", &tree, func);
	parser.build("select @ARGUMENTS from @TABLE where @KEY @OPERATOR @VALUE", &tree, func);
	parser.build("select @ARGUMENTS from @TABLE", &tree, func);

	parser.search("create table esgi.test (name VARCHAR)", &tree);
	// statistic(tree, 1);

	freeTree(tree);
	// free /* double free or corruption */

	return 0;
}

void func(char *s)
{
	printf("[%s]\n", s);
}
