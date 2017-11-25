#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

typedef struct
{
    char environment[256];
    int allowColor;
    int tree;
} Config;

Config settings(int, char **);

#endif // GLOBAL_H_INCLUDED
