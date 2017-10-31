#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#define true 1
#define false 0

typedef struct Node Node;
struct Node
{
	char data[256];
	char type[256];
	int size;
	struct Node **next;
	void (*callback)(char *s);
};

typedef struct Token Token;
struct Token
{
    char **data;
    size_t size;
};

typedef struct Parser Parser;
struct Parser
{
    void (*build)(char *, Node **, void (*callback)(char *s));
    void (*search)(char *, Node **);
};

Parser parserInit();
Node *initNode(char *, char *);
void freeTree(Node *);

#endif // PARSER_H_INCLUDED
