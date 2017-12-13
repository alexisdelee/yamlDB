#ifndef YAMLBDDGRAPHIQUE_H_INCLUDED
#define YAMLBDDGRAPHIQUE_H_INCLUDED

extern int ansiSupport;
extern int debug;

void quit(GtkWidget *window, gpointer data);
void callback_resultQuery(GtkWidget *buttonLaunch, gpointer data);

typedef struct
{
    GtkTextBuffer *queryResult;
    GtkTextBuffer *queryRecuperator;
    GtkWidget *inputArea;
    GtkWidget *outputArea;
    GtkWidget *window;
} Data;

#endif // YAMLBDDGRAPHIQUE_H_INCLUDED
