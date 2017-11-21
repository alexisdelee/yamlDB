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

#define YAML_INTEGER    1
#define YAML_REAL       2
#define YAML_CHARACTER  4
#define YAML_STRING     8
#define YAML_UNDEFINED  16

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
void _safeStrdup(char **, char *);
char* uniqueIdentifier(char *); // empty string for timestamp as seed
int _polyfillMkdir(char *);
int _polyfillRmdir(char *);
Path pathParse(char *, char *);
unsigned int _typeOf(char *);
int _isTypeOf(char *, int);
int contains(char *, char **, int);

#endif // TOOLBOX_H_INCLUDED
