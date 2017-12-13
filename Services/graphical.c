#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

#include "../Common/toolbox.h"
#include "../Common/throw.h"
#include "../Common/settings.h"
#include "../Common/Interface.h"
#include "../Common/colorShell.h"
#include "../Business/Parser.h"
#include "../Business/Callback.h"
#include "graphical.h"

Throw *interpreter(char *);

Throw *interpreter(char *sql)
{
    Node *tree = initNode(".", "root");
    Parser parser = parserInit();
    Throw *err;

    Callback callback = callbackInit();

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

    if(!strcmp(sql, "exit")) {
        gtk_main_quit();
        return NULL;
    } else if(!strcmp(sql, "help")) {
        err = setSuccess("");

        sprintf(err->output, "%s%-86s %s\n", err->output, "create database [DATABASE]", "create a database");
        sprintf(err->output, "%s%-86s %s\n", err->output, "drop database [DATABASE]", "completely delete a database and everything in it");
        sprintf(err->output, "%s%-86s %s\n", err->output, "create table [DATABASE.TABLE] [ARGUMENTS]", "create a table");
        sprintf(err->output, "%s%-86s %s\n", err->output, "drop table [DATABASE.TABLE]", "permanently delete a table");
        sprintf(err->output, "%s%-86s %s\n", err->output, "insert into [DATABASE.TABLE] values [ARGUMENTS]", "add records to a table");
        sprintf(err->output, "%s%-86s %s\n", err->output, "update [DATABASE.TABLE] set [KEY] = [VALUE] [where [KEY] [OPERATOR] [VALUE]]", "make changes to existing rows in a table");
        sprintf(err->output, "%s%-86s %s\n", err->output, "delete from [DATABASE.TABLE] [where [KEY] [OPERATOR] [VALUE]]", "delete rows from a table");
        sprintf(err->output, "%s%-86s %s\n", err->output, "select [ARGUMENTS] from [DATABASE.TABLE] [where [KEY] [OPERATOR] [VALUE]]", "retrieve data from a table");
        sprintf(err->output, "%s%-86s %s\n", err->output, "select [ARGUMENTS] from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = [DATABASE.TABLE]", "get information about all columns (TABLE_NAME / COLUMN_NAME / DATA_TYPE)");
        sprintf(err->output, "%s%-86s %s\n", err->output, "help", "display information about the SQL console");
        sprintf(err->output, "%s%-86s %s\n", err->output, "exit", "exit SQL console");
    } else {
        err = parser.sql(sql, &tree);
    }

    freeTree(tree);
    return err;
}

// fonction permettant de quitter "proprement" une application gtk
void quit(GtkWidget *window, gpointer data){
    gtk_main_quit();
}

// fonction permettant de récupérer la chaîne de caractère entrée et d'afficher un résultat dans la zone de sortie
void callback_resultQuery(GtkWidget *buttonLaunch, void *data) { // gpointer data){
    char *text;
    GtkTextIter start, end; // pointeurs sur la fin et le début du texte
    GtkStyleContext *context = gtk_widget_get_style_context(((Data *)data)->outputArea);

    gtk_text_buffer_get_bounds(((Data *)data)->queryRecuperator, &start, &end); // récupère le début et la fin de la chaine
    text = gtk_text_buffer_get_text(((Data *)data)->queryRecuperator, &start, &end, FALSE);

    Throw *err = interpreter(text);
    if(err) {
        if(err->err) {
            gtk_text_buffer_set_text(((Data *)data)->queryResult, (gchar *)err->message, -1);
            gtk_style_context_add_class(context, "error"); // ajout de la classe "error"
        } else {
            gtk_text_buffer_set_text(((Data *)data)->queryResult, (gchar *)err->output, -1);
            gtk_text_buffer_set_text(((Data *)data)->queryRecuperator, (gchar *)(""), -1); // on vide le champs uniquement si la commande est reconnue
            gtk_style_context_remove_class(context, "error"); // suppression de la classe "error"
        }

        free(err);
    }

    gtk_window_set_focus(GTK_WINDOW(((Data *)data)->window), ((Data *)data)->inputArea); // focus lors du démarrage de l'application

    _safeFree(&text);
}
