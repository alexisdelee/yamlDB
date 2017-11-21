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
    void (*create)(char *);
    void (*drop)(char *);
} Database;

typedef struct Table
{
    int (*load)(char *, char *, void *);
    void (*create)(char *, char *, void *);
    void (*insert)(char *, char *, void *);
    Stack *(*select)(char *, char *, void *, char *, char *, char *, ...);
    void (*update)(char *, char *, void *, char *, char *, char *, char *, char *);
    void (*delete)(char *, char *, void *, char *, char *, char *);
    void (*drop)(char *, char *);
} Table;

typedef struct
{
    Database database;
    Table table;
} Yaml;

Yaml yamlInit();

#endif // YAML_H_INCLUDED
