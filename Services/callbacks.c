#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

#include "yamlBddGraphique.h"

// fonction permettant de quitter "proprement" une application gtk
void quit(GtkWidget *window, gpointer data){
    gtk_main_quit();
}

// fonction permettant de récupérer la chaîne de caractère entrée et d'afficher un résultat dans la zone de sortie
void callback_resultQuery(GtkWidget *buttonLaunch, void *data) { // gpointer data){
    char *text;
    GtkTextIter start, end; // pointeurs sur la fin et le début du texte

    gtk_text_buffer_get_bounds(((Data *)data)->queryRecuperator, &start, &end); // récupère le début et la fin de la chaine
    text = gtk_text_buffer_get_text(((Data *)data)->queryRecuperator, &start, &end, FALSE);
    gtk_text_buffer_set_text(((Data *)data)->queryResult, (gchar*)text, -1);

    free(text);
}
