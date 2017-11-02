#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "dependancies.h"
#include "Parser.h"

void build(char *, Node **, void (*callback)(char *, void *));
void search(char *, Node **);
void statistic(Node *, int);
Token *_split(char *);
void addParameters(void *, ...);
int regexp(char *, char *, void *);

Parser parserInit()
{
    Parser parser;

    parser.build = build;
    parser.sql = search;
    parser.statistic = statistic;

    return parser;
};

Node *initNode(char *data, char *type)
{
	Node *seed = malloc(sizeof(Node));

	sprintf(seed->data, "%s", data);
	sprintf(seed->type, "%s", type);
	seed->size = 0;
	seed->next = NULL;
	seed->callback = NULL;

	return seed;
}

void build(char *s, Node **tree, void (*callback)(char *, void *))
{
	Token *token = _split(s);
	Node *seed = *tree;
	Node *_seed;
	int active, i, j;

	for(i = 0; i < token->size; i++) {
		active = 0;

		_seed = initNode(token->data[i], "node");

		if(_seed->data[0] == '@') {
			// sprintf(_seed->data, "%s", token->data[i] + 1);
			sprintf(_seed->type, "regexp");
		}

		if(i == token->size - 1) {
			_seed->callback = callback;
			_seed->next = NULL;
		}

		for(j = 0; j < seed->size; j++) {
			if(!strcmp(seed->next[j]->data, token->data[i])) {
				seed = seed->next[j];
				active = 1;
				break;
			}
		}

		if(!active) {
			seed->next = realloc(seed->next, sizeof(Node *) * ++seed->size);
			seed->next[seed->size - 1] = _seed;

			seed = seed->next[seed->size - 1];
		}
	}
}

void search(char *s, Node **tree)
{
	Token *token = _split(s);
	Token parameters;
	Node *seed = *tree;
	int active, i, j;

	parameters.data = NULL;
    parameters.size = 0;

	for(i = 0; i < token->size; i++) {
		active = false;

		if(seed->next) {
			for(j = 0; j < seed->size; j++) {
				if(!strcmp(seed->next[j]->type, "node") || !strcmp(seed->next[j]->type, "root")) {
					if(!strcmp(seed->next[j]->data, token->data[i])) {
						seed = seed->next[j]; // FIX
						active = true;
						break;
					}
				} else {
					if(regexp(seed->next[j]->data, token->data[i], &parameters)) {
						seed = seed->next[j];
						active = true;
						break;
					}
				}
			}
		} else {
			break;
		}
	}

	if(active && seed->next == NULL) {
		if(seed->callback) {
			seed->callback(s, &parameters);
		}

		for(i = 0; i < parameters.size; i++) {
			free(parameters.data[i]);
		}

		free(parameters.data);
	} else {
		printf("\"%s\" is not recognised as an internal command\n", s);
	}
}

void statistic(Node *seed, int depth)
{
	int i, j;
	char startCharacter;

	if(depth == 1) {
		printf(".\n");
	}

	if(seed) {
		for(i = 0; i < seed->size; i++) {
			startCharacter = i == seed->size - 1 ? '`' : '|';

			if(depth == 1) {
				printf("%c-- ", startCharacter);
			} else {
				for(j = 0; j < depth; j++) {
					printf("|   ");
				}

				printf("%c-- ", startCharacter);
			}

			printf("%s [%s]\n", seed->next[i]->data, seed->next[i]->type);
			statistic(seed->next[i], depth + 1);
		}
	}
}

void freeTree(Node *seed)
{
	int i;

	if(seed) {
		for(i = 0; i < seed->size; i++) {
			freeTree(seed->next[i]);
		}

		free(seed);
	}
}

Token *_split(char *buffer)
{
    Token *token = malloc(sizeof(Token));
    char *buf = malloc(sizeof(char) * (strlen(buffer) + 1));
    char *p, *start_of_word;
    int c;
    enum states { DULL, IN_WORD, IN_STRING, IN_PARENTHESIS } state = DULL;

    if(buf == NULL) {
        exit(-1);
    }

    buf[0] = '\0';
    strcpy(buf, buffer);

    token->data = NULL;
    token->size = 0;

    for (p = buf; *p != '\0'; p++) {
        c = (unsigned char) *p;

        switch (state) {
            case DULL:
                if (isspace(c)) {
                  continue;
                }

                if (c == '"') {
                    state = IN_STRING;
                    start_of_word = p + 1;

                    continue;
                } else if(c == '(') {
                    state = IN_PARENTHESIS;
                    start_of_word = p + 1;

                    continue;
                }

                state = IN_WORD;
                start_of_word = p;

                continue;
            case IN_STRING:
            case IN_PARENTHESIS:
                if (c == '"' || c == ')') {
                    *p = 0;

                    token->data = realloc(token->data, sizeof(char *) * ++token->size);
                    if(token->data == NULL) {
                        exit(-1);
                    }

                    token->data[token->size - 1] = strdup(start_of_word);
                    if(token->data[token->size - 1] == NULL) {
                        exit(-1);
                    }

                    state = DULL;
                }

                continue;
            case IN_WORD:
                if (isspace(c)) {
                    *p = 0;

                    token->data = realloc(token->data, sizeof(char *) * ++token->size);
                    if(token->data == NULL) {
                        exit(-1);
                    }

                    token->data[token->size - 1] = strdup(start_of_word);
                    if(token->data[token->size - 1] == NULL) {
                        exit(-1);
                    }

                    state = DULL;
                }

                continue;
        }
    }

    if (state != DULL)
    token->data = realloc(token->data, sizeof(Token) * token->size++);
    if(token->data == NULL) {
        exit(-1);
    }

    token->data[token->size - 1] = strdup(start_of_word);
    if(token->data[token->size - 1] == NULL) {
        exit(-1);
    }

    free(buf);

    return token;
}

void addParameters(void *_parameters, ...)
{
    Token *parameters = (Token *)_parameters;
	va_list args;
	char *data;

	va_start(args, _parameters);

	while((data = va_arg(args, char *))) {
		parameters->data = realloc(parameters->data, sizeof(char *) * ++parameters->size);
		if(parameters->data == NULL) {
			exit(-1);
		}

		parameters->data[parameters->size - 1] = strdup(data);
		if(parameters->data[parameters->size - 1] == NULL) {
			exit(-1);
		}
	}

	va_end(args);
}

int regexp(char *rgx, char *str, void *_parameters)
{
	char string[10][256];
	int status;

	if(!strcmp(rgx, "@DATABASE") || !strcmp(rgx, "@KEY") || !strcmp(rgx, "@VALUE")) {
		status = sscanf(str, "%255s", string[0]);
		fflush(stdin);

		if(status == 1) {
            addParameters(_parameters, string[0], NULL);
			return true;
		}
	} else if(!strcmp(rgx, "@TABLE")) {
		status = sscanf(str, "%255[^.].%255s", string[0], string[1]);
		fflush(stdin);

		if(status == 2) {
            addParameters(_parameters, string[0], string[1], NULL);
			return true;
		}
	} else if(!strcmp(rgx, "@ARGUMENTS")) {
		/* status = sscanf(str, "%255s", string[0]); // TODO
		fflush(stdin);

		if(status == 1) {
            addParameters(parameters, string[0], NULL);
			return true;
		} */

		addParameters(_parameters, str, NULL);
		return true;
	} else if(!strcmp(rgx, "@OPERATOR")) {
		if(!strcmp(str, "=") || !strcmp(str, "<>")) {
            addParameters(_parameters, str, NULL);
			return true;
		}
	} else {
		printf("Exception: unknown REGEXP\n");
	}

	return false;
}
