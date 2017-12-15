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
    GtkWidget *principalContainer = NULL;
    GtkWidget *secondaryContainer = NULL;
    GtkWidget *buttonLaunch = NULL;
    GtkCssProvider *cssProvider;
    Data data;

    // initialisation des composants et création de la fenêtre
    gtk_init(&argc, &argv);

    data.window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //Création de la fenêtre
    gtk_window_set_title(GTK_WINDOW(data.window), "YamlBdd");

    principalContainer = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    secondaryContainer = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    data.inputArea = gtk_text_view_new();
    data.outputArea = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(data.outputArea), FALSE);

    data.queryRecuperator = gtk_text_view_get_buffer(GTK_TEXT_VIEW (data.inputArea));
    data.queryResult = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.outputArea));
    buttonLaunch = gtk_button_new_with_label("Execute");

    // ajouter les différentes parties dans la fenêtre
    gtk_paned_pack1(GTK_PANED(secondaryContainer), data.inputArea, FALSE, TRUE);
    gtk_paned_pack2(GTK_PANED(secondaryContainer), buttonLaunch, FALSE, FALSE);
    gtk_paned_pack1(GTK_PANED(principalContainer), secondaryContainer, FALSE, TRUE);
    gtk_paned_pack2(GTK_PANED(principalContainer), data.outputArea, FALSE, TRUE);

    // dimensionne les panneaux et la fenêtre
    gtk_paned_set_position(GTK_PANED(secondaryContainer), 1000);
    gtk_paned_set_position(GTK_PANED(principalContainer), 50);
    gtk_container_add(GTK_CONTAINER(data.window), principalContainer);
    gtk_window_set_default_size(GTK_WINDOW(data.window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER (data.window), 1);
    // gtk_window_maximize(GTK_WINDOW(data.window));

    // charger les styles CSS
    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "ui.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_window_set_focus(GTK_WINDOW(data.window), data.inputArea); // focus lors du démarrage de l'application

    // déclaration des callback
    g_signal_connect(G_OBJECT(buttonLaunch), "clicked", G_CALLBACK(callback_resultQuery), &data);
    g_signal_connect(data.window, "destroy", G_CALLBACK(quit), NULL);

    // affichage de la fenêtre et lancement de la main loop
    gtk_widget_show_all (data.window);
    gtk_main();

    return 0;
}
