#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

typedef struct
{
    int size;
    unsigned char *type;
    char **data;
} Header;

typedef struct
{
    int size;
    char status;
    char id[5];
    char **data;
} Core;

typedef struct
{
    Core **(*newStack)();
    int (*initializer)(void *, unsigned char, char *);
    int (*push)(void *, char *, char *);
    int (*commit)(char *, Header *, Core *);
    int (*load)(void *, char *);
    void (*reload)(void *, char *);
    void (*set)(void *, int, int, char *);
    void (*remove)(void *, int);
} Method;

typedef struct
{
    int length;
    Header *header;
    Core **core;
    Method _;
} Entity;

Entity *entityInit();

void freeEntity(Entity *);

#endif // ENTITY_H_INCLUDED
