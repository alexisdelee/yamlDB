#ifndef YAML_H_INCLUDED
#define YAML_H_INCLUDED

typedef struct
{
    char *key;
    char *value;
    char *operator;
} WhereStatement;

typedef struct
{
    char **keys;
    int size;
    WhereStatement where;
} SelectStatement;

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
    void *(*create)(char *);
    void *(*drop)(char *);
} Database;

typedef struct Table
{
    void *(*load)(char *, char *, void *);
    void *(*create)(char *, char *, void *);
    void *(*insert)(char *, char *, void *);
    void *(*select)(char *, char *, void *, void **, SelectStatement);
    void *(*update)(char *, char *, void *, char *, char *, char *, char *, char *);
    void *(*delete)(char *, char *, void *, char *, char *, char *);
    void *(*drop)(char *, char *);
} Table;

typedef struct
{
    Database database;
    Table table;
} Yaml;

Yaml yamlInit();
void destroyStack(Stack *);
void addStatement(SelectStatement *, char *);
void whereStatement(SelectStatement *, char *, char *, char *);
void freeStatement(SelectStatement *);

#endif // YAML_H_INCLUDED
