#ifndef TOOLBOX_H_INCLUDED
#define TOOLBOX_H_INCLUDED

#define true 1
#define false 0

#ifdef __unix__
    #define OS_Windows false
#elif defined(_WIN32) || defined(WIN32)
    #define OS_Windows true
#endif

#define ROOT "container"
#define EXTNAME ".yaml"

typedef struct Path Path;
struct Path
{
    char root[256]; // "container",
    char dir[261];  // "container/40d5"
    char base[261]; // "e0c5.yaml"
    char ext[256];  // ".yaml"
    char name[5];   // "e055"
    char path[522]; // "container/40d5/e0c5.yaml"
};

void _safeFree(char **);
char* uniqueIdentifier(char *); // empty string for timestamp as seed
int _polyfillMkdir(char *);
int _polyfillRmdir(char *);
Path pathParse(char *, char *);

#endif // TOOLBOX_H_INCLUDED
