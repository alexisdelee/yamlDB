#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/Parser.h"

void func(char *);

int main(int argc, char **argv)
{
	Node *tree = initNode(".", "root");

	build("CREATE DATABASE @DATABASE", &tree, func);
	build("DROP DATABASE @DATABASE", &tree, func);
	build("CREATE TABLE @TABLE @ARGUMENTS", &tree, func);
	build("DROP TABLE @TABLE", &tree, func);
	build("INSERT INTO @TABLE VALUES @ARGUMENTS", &tree, func);
	build("UPDATE @TABLE SET @KEY = @VALUE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	build("DELETE FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	build("SELECT @ARGUMENTS FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, func);
	build("SELECT @ARGUMENTS FROM @TABLE", &tree, func);

	/* ---------- */

	build("create database @DATABASE", &tree, func);
	build("drop database @DATABASE", &tree, func);
	build("create table @TABLE @ARGUMENTS", &tree, func);
	build("drop table @TABLE", &tree, func);
	build("insert into @TABLE values @ARGUMENTS", &tree, func);
	build("update @TABLE set @KEY = @VALUE where @KEY @OPERATOR @VALUE", &tree, func);
	build("delete from @TABLE where @KEY @OPERATOR @VALUE", &tree, func);
	build("select @ARGUMENTS from @TABLE where @KEY @OPERATOR @VALUE", &tree, func);
	build("select @ARGUMENTS from @TABLE", &tree, func);

	search("create table esgi.test (name VARCHAR)", &tree);
	// statistic(tree, 1);

	freeTree(tree);
	// free /* double free or corruption */

	return 0;
}

void func(char *s)
{
	printf("[%s]\n", s);
}
