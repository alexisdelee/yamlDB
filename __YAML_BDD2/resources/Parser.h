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

Node *initNode(char *, char *);
void build(char *s, Node **, void (*callback)(char *s));
void search(char *s, Node **);
void statistic(Node *, int);
void freeTree(Node *);
Token *_split(char *);
int regexp(char *, char *);

#endif // PARSER_H_INCLUDED
