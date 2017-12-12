#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

extern int debug;

typedef struct Node Node;
struct Node
{
	char data[256];
	char type[256];
	int size;
	struct Node **next;
	void *(*callback)(char *, void *);
};

typedef struct
{
    void (*build)(char *, Node **, void *(*callback)(char *, void *));
    void *(*sql)(char *, Node **);
    void (*statistic)(Node *, int);
} Parser;

Parser parserInit();
Node *initNode(char *, char *);
void freeTree(Node *);

#endif // PARSER_H_INCLUDED
