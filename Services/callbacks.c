#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "yamlBddGraphique.h"

extern GtkTextBuffer *queryResult;
extern GtkTextBuffer * queryRecuperator;

//fonction permettant de quitter "proprement" une application gtk
void quit(GtkWidget *window, gpointer data){
    gtk_main_quit();
}

//fonction permettant de r�cup�rer la cha�ne de caract�re entr�e et d'afficher un r�sultat dans la zone de sortie
void callback_resultQuery(GtkWidget *buttonLaunch, gpointer data){
    char *text;
    GtkTextIter start, end; //pointeurs sur la fin et le d�but du texte

    gtk_text_buffer_get_bounds (queryRecuperator, &start, &end); //r�cup�re le d�but et la fin de la chaine
    text = gtk_text_buffer_get_text (queryRecuperator, &start, &end, FALSE);
    gtk_text_buffer_set_text(queryResult, (gchar*)text, -1);
    free(text);
}
