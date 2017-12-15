#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Common/toolbox.h"
#include "../../Common/throw.h"
#include "../../Common/settings.h"
#include "../../Common/Interface.h"
#include "../../Common/colorShell.h"
#include "../Parser.h"
#include "../Callback.h"

int ansiSupport;
int debug;

int main(int argc, char **argv)
{
    Node *tree = initNode(".", "root");
    Parser parser = parserInit();
    Callback callback = callbackInit();
    Interface interface = interfaceInit();
    Throw *err = NULL;
    char *sql = NULL;

    Settings settings = getSettings();
    ansiSupport = settings.allowColor;
    debug = settings.debug;

    parser.build("CREATE DATABASE @DATABASE", &tree, callback.database.create);
    parser.build("DROP DATABASE @DATABASE", &tree, callback.database.drop);
    parser.build("CREATE TABLE @TABLE @ARGUMENTS", &tree, callback.table.create);
    parser.build("DROP TABLE @TABLE", &tree, callback.table.drop);
    parser.build("INSERT INTO @TABLE VALUES @ARGUMENTS", &tree, callback.table.insert);
    parser.build("UPDATE @TABLE SET @KEY = @VALUE WHERE @KEY @OPERATOR @VALUE", &tree, callback.table.update);
    parser.build("UPDATE @TABLE SET @KEY = @VALUE", &tree, callback.table.update);
    parser.build("DELETE FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, callback.table.delete);
    parser.build("DELETE FROM @TABLE", &tree, callback.table.delete);
    parser.build("SELECT @ARGUMENTS FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = @TABLE", &tree, callback.table.information);
    parser.build("SELECT @ARGUMENTS FROM @TABLE WHERE @KEY @OPERATOR @VALUE", &tree, callback.table.selectWhere);
    parser.build("SELECT @ARGUMENTS FROM @TABLE", &tree, callback.table.select);

    /* ---------- */

    parser.build("create database @DATABASE", &tree, callback.database.create);
    parser.build("drop database @DATABASE", &tree, callback.database.drop);
    parser.build("create table @TABLE @ARGUMENTS", &tree, callback.table.create);
    parser.build("drop table @TABLE", &tree, callback.table.drop);
    parser.build("insert into @TABLE values @ARGUMENTS", &tree, callback.table.insert);
    parser.build("update @TABLE set @KEY = @VALUE where @KEY @OPERATOR @VALUE", &tree, callback.table.update);
    parser.build("update @TABLE set @KEY = @VALUE", &tree, callback.table.update);
    parser.build("delete from @TABLE where @KEY @OPERATOR @VALUE", &tree, callback.table.delete);
    parser.build("delete from @TABLE", &tree, callback.table.delete);
    parser.build("select @ARGUMENTS from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = @TABLE", &tree, callback.table.information);
    parser.build("select @ARGUMENTS from @TABLE where @KEY @OPERATOR @VALUE", &tree, callback.table.selectWhere);
    parser.build("select @ARGUMENTS from @TABLE", &tree, callback.table.select);

    if(settings.tree) {
        parser.statistic(tree, 1);
        printf("\n");
    }

    while(true) {
        if(sql != NULL) free(sql);

        sql = interface.input("sql> ");
        if(!strcmp(sql, "exit")) {
            success("\nClosing the sql console...\n");
            break;
        } else if(!strcmp(sql, "help")) {
            success("%-86s %s\n", "create database [DATABASE]", "create a database");
            success("%-86s %s\n", "drop database [DATABASE]", "completely delete a database and everything in it");
            success("%-86s %s\n", "create table [DATABASE.TABLE] [ARGUMENTS]", "create a table");
            success("%-86s %s\n", "drop table [DATABASE.TABLE]", "permanently delete a table");
            success("%-86s %s\n", "insert into [DATABASE.TABLE] values [ARGUMENTS]", "add records to a table");
            success("%-86s %s\n", "update [DATABASE.TABLE] set [KEY] = [VALUE] [where [KEY] [OPERATOR] [VALUE]]", "make changes to existing rows in a table");
            success("%-86s %s\n", "delete from [DATABASE.TABLE] [where [KEY] [OPERATOR] [VALUE]]", "delete rows from a table");
            success("%-86s %s\n", "select [ARGUMENTS] from [DATABASE.TABLE] [where [KEY] [OPERATOR] [VALUE]]", "retrieve data from a table");
            success("%-86s %s\n", "select [ARGUMENTS] from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = [DATABASE.TABLE]", "get information about all columns (TABLE_NAME / COLUMN_NAME / DATA_TYPE)");
            success("%-86s %s\n", "help", "display information about the SQL console");
            success("%-86s %s\n", "exit", "exit SQL console");

            continue;
        }

        sql = cheatForSelect(sql);
        err = (Throw *)parser.sql(sql, &tree);

        if(err->err) {
            printStackError(err, debug);
        } else {
            success(err->output);
        }

        strcpy(err->output, "");
        free(err);
    }

    free(sql);
    freeTree(tree);

    return 0;
}
