#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Common/toolbox.h"
#include "../../Common/throw.h"
#include "../../Common/settings.h"
#include "../Parser.h"
#include "../Callback.h"

int ansiSupport;
int debug;

int main(int argc, char **argv)
{
    Node *tree = initNode(".", "root");
    Parser parser = parserInit();
    Callback callback = callbackInit();

    Settings settings = getSettings();
    ansiSupport = settings.allowColor;
    debug = settings.debug;

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

    if(settings.tree) {
        parser.statistic(tree, 1);
        printf("\n");
    }

    parser.sql("delete from esgi.test where age = 19", &tree);
    freeTree(tree);

    return 0;
}
