#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "../../Common/toolbox.h"
#include "../../Common/throw.h"
#include "../../Common/settings.h"
#include "../../Common/Interface.h"
#include "../../Common/colorShell.h"
#include "../../Business/Parser.h"
#include "../../Business/Callback.h"
#include "../graphical.h"

int ansiSupport;
int debug;

int main (int argc, char **argv)
{
    GtkWidget *window = NULL;
    GtkWidget *principalContainer = NULL;
    GtkWidget *secondaryContainer = NULL;
    GtkWidget *textArea = NULL;
    GtkWidget *textAreaResult = NULL;
    GtkWidget *buttonLaunch = NULL;

    Settings settings = getSettings();
    Data data;
    data.settings = &settings;

    // printf("%d\n", ((Settings *)data.settings)->allowColor);

    // initialisation des composants et création de la fenêtre
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //Création de la fenêtre
    gtk_window_set_title(GTK_WINDOW(window), "YamlBdd");

    principalContainer = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    secondaryContainer = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    textArea = gtk_text_view_new();
    textAreaResult = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textAreaResult), FALSE);

    data.queryRecuperator = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textArea));
    data.queryResult = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textAreaResult));
    buttonLaunch = gtk_button_new_with_label("Execute");

    // ajouter les différentes parties dans la fenêtre
    gtk_paned_pack1(GTK_PANED(secondaryContainer), textArea, FALSE, TRUE);
    gtk_paned_pack2(GTK_PANED(secondaryContainer), buttonLaunch, FALSE, FALSE);
    gtk_paned_pack1(GTK_PANED(principalContainer), secondaryContainer, FALSE, TRUE);
    gtk_paned_pack2(GTK_PANED(principalContainer), textAreaResult, FALSE, TRUE);

    // dimensionne les panneaux et la fenêtre
    gtk_paned_set_position(GTK_PANED(secondaryContainer), 1000);
    gtk_paned_set_position(GTK_PANED(principalContainer), 50);
    gtk_container_add(GTK_CONTAINER(window), principalContainer);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER (window), 1);
    gtk_window_maximize(GTK_WINDOW(window));

    // déclaration des callback
    g_signal_connect(G_OBJECT(buttonLaunch), "clicked", G_CALLBACK(callback_resultQuery), &data);
    g_signal_connect(window, "destroy", G_CALLBACK(quit), NULL);

    // affichage de la fenêtre et lancement de la main loop
    gtk_widget_show_all (window);
    gtk_main();

    return 0;
}
