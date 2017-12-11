#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

#include "yamlBddGraphique.h"

// fonction permettant de quitter "proprement" une application gtk
void quit(GtkWidget *window, gpointer data){
    gtk_main_quit();
}

// fonction permettant de r�cup�rer la cha�ne de caract�re entr�e et d'afficher un r�sultat dans la zone de sortie
void callback_resultQuery(GtkWidget *buttonLaunch, void *data) { // gpointer data){
    char *text;
    GtkTextIter start, end; // pointeurs sur la fin et le d�but du texte

    gtk_text_buffer_get_bounds(((Data *)data)->queryRecuperator, &start, &end); // r�cup�re le d�but et la fin de la chaine
    text = gtk_text_buffer_get_text(((Data *)data)->queryRecuperator, &start, &end, FALSE);
    gtk_text_buffer_set_text(((Data *)data)->queryResult, (gchar*)text, -1);

    free(text);
}
