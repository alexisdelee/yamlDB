#ifndef YAML_H_INCLUDED
#define YAML_H_INCLUDED

typedef struct
{
    int *active;
    int size;
    int *ids;
} Indexed;

typedef struct
{
    int currentPointer;
    int size;
    Indexed *indexed;
} Stack;

typedef struct
{
    int (*create)(char *);
    int (*drop)(char *);
} Database;

typedef struct Table
{
    int (*load)(char *, char *, void *);
    int (*create)(char *, char *, void *);
    int (*insert)(char *, char *, void *);
    Stack *(*select)(char *, char *, void *, char *, char *, char *, ...);
    int (*update)(char *, char *, void *, char *, char *, char *, char *, char *);
    int (*delete)(char *, char *, void *, char *, char *, char *);
    int (*drop)(char *, char *);
} Table;

typedef struct
{
    Database database;
    Table table;
} Yaml;

Yaml yamlInit();
void destroyStack(Stack *);

#endif // YAML_H_INCLUDED
