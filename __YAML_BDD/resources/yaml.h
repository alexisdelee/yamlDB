#ifndef YAML_H_INCLUDED
#define YAML_H_INCLUDED

/* typedef struct Database Database;
struct Database
{
    void (*create)(char *);
    void (*drop)(char *);
};

typedef struct Table Table;
struct Table
{
    void (*create)(char *, char *);
    void (*drop)(char *, char *);
};

typedef struct Yaml Yaml;
struct Yaml
{
    Database database;
    Table table;
}; */

typedef struct
{
    void (*create)(char *);
    void (*drop)(char *);
} Database;

typedef struct Table
{
    void (*load)(char *, char *, void *);
    void (*create)(char *, char *, void *);
    void (*insert)(char *, char *, void *);
    void (*drop)(char *, char *);
} Table;

typedef struct
{
    Database database;
    Table table;
} Yaml;

Yaml yamlInit();

#endif // YAML_H_INCLUDED
