#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

typedef struct
{
    int allowColor;
    int tree;
    int debug;
} Settings;

Settings getSettings();

#endif // GLOBAL_H_INCLUDED
