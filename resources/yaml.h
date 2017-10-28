#ifndef YAML_H_INCLUDED
#define YAML_H_INCLUDED

typedef struct Database Database;
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
};

Yaml yamlInit();

#endif // YAML_H_INCLUDED
