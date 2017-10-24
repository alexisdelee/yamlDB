#ifndef INTERFACE_H_INCLUDED

#define INTERFACE_H_INCLUDED

#define LABEL_BULLET_POINT 0x01
#define LABEL_NUMBERING 0x02

typedef struct Options Options;
struct Options
{
    char *title;
    char *label;
    int ansiSupport;
    int quit;
};

typedef struct Interface Interface;
struct Interface
{
    void (*load)(Options *, char *);
    int (*prompt)(int, void (*callback)(char *, int), Options *, ...);
    char *(*input)(char *);
    Options options;
};

Interface interfaceInit();

#endif // INTERFACE_H_INCLUDED
