#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toolbox.h"
#include "colorShell.h"
#include "Engine.h"
#include "Entity.h"

Header *newHeader();
Core **newCore();
Method MethodInit();
int pushHeader(void *, unsigned char, char *);
int pushCore(void *, char *, char *);
void parseYaml(Entity *, char *, int, int, char *);
void loadYaml(void *, char *);
void commitEntity(char *, Header *, Core *);
void rewriteYaml(void *, char *);
void setCore(void *, int, int, char *);
void removeCore(void *_entity, int line);

Method MethodInit()
{
    Method method;

    method.newStack = newCore;
    method.initializer = pushHeader;
    method.push = pushCore;
    method.commit = commitEntity;
    method.load = loadYaml;
    method.reload = rewriteYaml;
    method.set = setCore;
    method.remove = removeCore;

    return method;
}

Entity *entityInit()
{
    Entity *entity = malloc(sizeof(Entity));
    if(entity == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    entity->length = 0;
    entity->header = newHeader();

    entity->core = NULL;
    entity->core = newCore(entity);

    entity->_ = MethodInit();

    return entity;
}

// privates methods

Header *newHeader()
{
    Header *header = malloc(sizeof(Header));
    if(header == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    header->size = 0;
    header->type = NULL;
    header->data = NULL;

    return header;
}

Core **newCore(Entity *entity)
{
    if(entity->core == NULL) {
        entity->core = malloc(sizeof(Core *) * ++entity->length);
    } else {
        entity->core = realloc(entity->core, sizeof(Core *) * ++entity->length);
    }

    if(entity->core == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    entity->core[entity->length - 1] = malloc(sizeof(Core));
    if(entity->core[entity->length - 1] == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    entity->core[entity->length - 1]->size = 0;
    entity->core[entity->length - 1]->data = NULL;

    return entity->core;
}

int pushHeader(void *_entity, unsigned char type, char *data)
{
    Entity *entity = (Entity *)_entity;
    int i;

    for(i = 0; i < entity->header->size; i++) {
        if(!strcmp(entity->header->data[i], data)) { // check if a column with this name already exists
            return false;
        }
    }

    entity->header->size++;

    entity->header->type = realloc(entity->header->type, sizeof(unsigned char) * entity->header->size);
    if(entity->header->type == NULL) {
        danger(true, "Exception: error with malloc\n");
    } else {
        entity->header->type[entity->header->size - 1] = type;
    }

    entity->header->data = realloc(entity->header->data, sizeof(char *) * entity->header->size);
    if(entity->header->data == NULL) {
        danger(true, "Exception: error with malloc\n");
    } else {
        entity->header->data[entity->header->size - 1] = strdup(data);
        if(entity->header->data[entity->header->size - 1] == NULL) {
            danger(true, "Exception: error with malloc\n");
        }
    }

    return true;
}

int pushCore(void *_entity, char *data, char *id)
{
    Entity *entity = (Entity *)_entity;
    char *generateId = uniqueIdentifier("");

    if(entity->core[entity->length - 1]->size + 1 > entity->header->size) {
        danger(false, "Exception: overflow\n");
        return false;
    } else if(!_isTypeOf(data, (int)entity->header->type[entity->core[entity->length - 1]->size])) {
        printf("[%s|%d]\n", data, (int)entity->header->type[entity->core[entity->length - 1]->size]);
        danger(false, "Exception: this datatype is not expected for this data\n");
        return false;
    }

    entity->core[entity->length - 1]->size++;

    entity->core[entity->length - 1]->data = realloc(entity->core[entity->length - 1]->data, sizeof(char *) * entity->core[entity->length - 1]->size);
    if(entity->core[entity->length - 1]->data == NULL) {
        danger(true, "Exception: error with malloc\n");
    } else {
        if(id == NULL) {
            sprintf(entity->core[entity->length - 1]->id, "%s", generateId);
        } else {
            sprintf(entity->core[entity->length - 1]->id, "%.4s", id);
        }

        entity->core[entity->length - 1]->data[entity->core[entity->length - 1]->size - 1] = strdup(data);
        if(entity->core[entity->length - 1]->data[entity->core[entity->length - 1]->size - 1] == NULL) {
            danger(true, "Exception: error with malloc\n");
        }

        if(entity->core[entity->length - 1]->size == entity->header->size) {
            entity->core[entity->length - 1]->status = 'A'; // added to index
        } else {
            entity->core[entity->length - 1]->status = '!'; // ignored
        }
    }

    _safeFree(&generateId);
    return true;
}

void parseYaml(Entity *entity, char *data, int start, int end, char *status)
{
    int i;
    char *_data = NULL;
    char *source = malloc(sizeof(char) * (end - start + 1));
    if(source == NULL) {
        danger(true, "Exception: error with malloc\n");
    } else {
        source[0] = '\0';
    }

    sprintf(source, "%.*s", end - start, data + start);

    _data = strtok(source, ",");
    for(i = 0; _data != NULL; i++) {
        if(!strcmp(status, "header_data")) {
            sprintf(_data, "%.*s", strlen(_data) - 2, _data + 1);
            entity->_.initializer(entity, YAML_STRING, _data);
        } else if(!strcmp(status, "header_types")) {
            if(i < entity->header->size) {
                entity->header->type[i] = (unsigned char)(_data[0] - '0'); // convert char to true number
            }
        } else {
            if(!i && ((entity->length > 1) || (entity->length == 1 && entity->core[0]->size))) {
                entity->core = entity->_.newStack(entity);
            }

            sprintf(_data, "%.*s", strlen(_data) - 2, _data + 1);
            entity->_.push(entity, _data, data);

            entity->core[entity->length - 1]->status = ' '; // not updated
        }

        _data = strtok(NULL, ",");
    }

    _safeFree(&source);
}

void loadYaml(void *_entity, char *tablePath)
{
    Entity *entity = (Entity *)_entity;
    FILE *tableFile = NULL;
    char data[501];
    int index = 0;
    int size;

    tableFile = fopen(tablePath, "r");
    if(tableFile == NULL) {
        danger(true, "Exception: unknown error\n");
    }

    while(fgets(data, 501, tableFile) != NULL) {
        size = strlen(data);
        if(data[size - 1] == '\n') {
            data[size - 1] = '\0';
            size--;
        }

        switch(index) {
            case 0:
                break;
            case 1:
                parseYaml(entity, data, 11, size - 1, "header_data");
                break;
            case 2:
                parseYaml(entity, data, 12, size - 1, "header_types");
                break;
            default:
                if(size > 0) {
                    parseYaml(entity, data, strcspn(data, "[") + 1, size - 1, "core");
                }
        }

        index += 1;
    }

    fclose(tableFile);
}

void commitEntity(char *tablePath, Header *header, Core *core)
{
    FILE *tableFile = NULL;
    int i, j;

    tableFile = fopen(tablePath, header == NULL ? "a" : "w");
    if(tableFile == NULL) {
        danger(false, "Exception: unknown error\n");
        return;
    }

    if(header != NULL) {
        fprintf(tableFile, "proto:\n");

        for(i = 0; i < 2; i++) {
            for(j = -1; j < header->size; j++) {
                if(i == 0) {
                    if(j == -1) {
                        fprintf(tableFile, "  - data: [");
                        continue;
                    }

                    fprintf(tableFile, "\"%s\"%s", header->data[j], j == header->size - 1 ? "" : ",");
                } else {
                    if(j == -1) {
                        fprintf(tableFile, "  - types: [");
                        continue;
                    }

                    fprintf(tableFile, "%d%s", header->type[j], j == header->size - 1 ? "" : ",");
                }
            }

            fprintf(tableFile, "]\n");
        }
    } else if(core != NULL) {
        fprintf(tableFile, "\n%s: [", core->id);

        for(i = 0; i < core->size; i++) {
            fprintf(tableFile, "\"%s\"%s", core->data[i], i == core->size - 1 ? "" : ",");
        }

        fprintf(tableFile, "]");
    }

    fclose(tableFile);
}

void rewriteYaml(void *_entity, char *tablePath)
{
    Entity *entity = (Entity *)_entity;
    int i;

    commitEntity(tablePath, entity->header, NULL);

    for(i = 0; i < entity->length; i++) {
        if(entity->core[i]->status != 'D') {
            commitEntity(tablePath, NULL, entity->core[i]);
        }
    }
}

void setCore(void *_entity, int line, int col, char *data)
{
    Entity *entity = (Entity *)_entity;

    if(line >= 0 && line < entity->length) {
        if(col >= 0 && col < entity->core[line]->size) {
            if(entity->core[line]->data[col] != NULL) {
                free(entity->core[line]->data[col]);
            }

            entity->core[line]->data[col] = strdup(data);
            if(entity->core[line]->data[col] == NULL) {
                danger(true, "Exception: error with malloc\n");
            }

            entity->core[line]->status = 'M';
        }
    }
}

void removeCore(void *_entity, int line)
{
    Entity *entity = (Entity *)_entity;

    if(line >= 0 && line < entity->length) {
        entity->core[line]->status = 'D';
    }
}

void freeEntity(Entity *entity)
{
	int i, j;

	for(i = 0; i < entity->length; i++) {
        for(j = 0; j < entity->core[i]->size; j++) {
            _safeFree(&entity->core[i]->data[j]);
        }

        if(entity->core[i]->data != NULL) free(entity->core[i]->data);
	}

	if(entity->core != NULL) free(entity->core);
	if(entity->header != NULL) free(entity->header);
	if(entity != NULL) free(entity);
}
