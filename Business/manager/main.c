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
        }

        parser.sql(sql, &tree);
    }

    free(sql);
    freeTree(tree);

    return 0;
}
