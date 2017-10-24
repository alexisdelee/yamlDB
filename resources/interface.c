#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

#include "toolbox.h"
#include "colorShell.h"
#include "interface.h"

// private functions

void interfaceLoad(Options *, char *);
int interfacePrompt(int, void (*callback)(char *, int), Options *, ...);
char *interfaceInput(char *);
void _trim(char *);
int _getInteger(char *);
void _garbageCollector(char ***, int);
void _free(Options *);

Interface interfaceInit()
{
    Interface interface;

    const char *defaultTitle = "Starting the database shell console...\n";

    if((interface.options.title = strdup(defaultTitle)) == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    interface.options.label = ">";
    interface.options.quit = false;

    interface.load = interfaceLoad;
    interface.prompt = interfacePrompt;
    interface.input = interfaceInput;

    return interface;
}

void interfaceLoad(Options *options, char *configPath)
{
    FILE *configFile = fopen(configPath, "r");
    char line[501], *command = NULL;
    int delimiter;

    if(configFile == NULL) {
        danger(false, "Exception: error with the configuration file \"%s\"\nIgnored file\n\n", configPath);
    }

    while(fgets(line, 501, configFile)) {
        _trim(&line[0]); // \r or \n for Mac, Unix and Windows
        if(OS_Windows) {
            _trim(&line[0]); // \n for Windows
        }

        if((delimiter = strcspn(line, "=")) == strlen(line)) {
            continue;
        }

        command = malloc(sizeof(char) * (delimiter + 1));
        if(command == NULL) {
            danger(true, "Exception: error with malloc\n");
        } else {
            strncpy(command, line, delimiter);
            command[delimiter] = '\0';
        }

        if(!strcmp(command, "title")) {
            _safeFree(&options->title);

            if((options->title = strdup(line + delimiter + 1)) == NULL) {
                danger(true, "Exception: error with malloc\n");
            }
        } else if(!strcmp(command, "quit")) {
            if(!strcmp(line + delimiter + 1, "false")) {
                options->quit = false;
            } else if(!strcmp(line + delimiter + 1, "true")) {
                options->quit = true;
            }
        }

        _safeFree(&command);
        command = NULL;
    }

    fclose(configFile);
}

int interfacePrompt(int welcomeMode, void (*callback)(char *, int), Options *options, ...)
{
    char *choice;
    char **choices = malloc(sizeof(char *));
    int counter = 0;
    int index;
    int isExitOption = false;
    const int LIMIT_STRING = (int)floor(log10(counter)) + 1;
    char response[256];
    va_list args;

    if(choices == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    va_start(args, options);

    if(welcomeMode) {
        welcome("%s\n", options->title);

        printf(" /$$     /$$ /$$$$$$  /$$      /$$ /$$             /$$$$$$$  /$$$$$$$ \n");
        printf("|  $$   /$$//$$__  $$| $$$    /$$$| $$            | $$__  $$| $$__  $$ \n");
        printf(" \\  $$ /$$/| $$  \\ $$| $$$$  /$$$$| $$            | $$  \\ $$| $$  \\ $$ \n");
        printf("  \\  $$$$/ | $$$$$$$$| $$ $$/$$ $$| $$            | $$  | $$| $$$$$$$ \n");
        printf("   \\  $$/  | $$__  $$| $$  $$$| $$| $$            | $$  | $$| $$__  $$ \n");
        printf("    | $$   | $$  | $$| $$\\  $ | $$| $$            | $$  | $$| $$  \\ $$ \n");
        printf("    | $$   | $$  | $$| $$ \\/  | $$| $$$$$$$$      | $$$$$$$/| $$$$$$$/ \n");
        printf("    |__/   |__/  |__/|__/     |__/|________/      |_______/ |_______/ \n\n");
    }

    while((choice = va_arg(args, char *))) {
        if((choices[counter] = malloc(sizeof(char) * 501)) == NULL) {
            danger(true, "Exception: error with malloc\n");
        } else {
            sprintf(choices[counter], "%s", choice);
        }


        counter++;
        if(welcomeMode) {
            warningList("[%d] %s\n", counter, choice);
        }

        if((choices = realloc(choices, sizeof(char *) * (counter + 1))) == NULL) {
            danger(true, "Exception: error with malloc\n");
        }
    }

    if(options->quit) {
        if(welcomeMode) {
            warningList("\n[%d] Exit interpreter\n", 0);
        }

        isExitOption = true;
    }

    va_end(args);

    if(welcomeMode) {
        printf("\n");
    }

    do {
        infoScan("#? ");
        fgets(response, 256, stdin);
        fflush(stdin); // clean stdin

        _trim(&response[0]);
        if(strlen(response) > LIMIT_STRING) {
            continue;
        }

        index = _getInteger(&response[0]); // cast char[] to char *
        if(isExitOption && index == 0) {
            _garbageCollector(&choices, counter + 1);
            _free(options);

            return false;
        }
    } while(index < 1 || index > counter);

    // printf("[before callback]\n");
    callback(choices[index - 1], index);

    // printf("[before garbage collector]\n");
    _garbageCollector(&choices, counter + 1);

    // printf("[after garbage collector]\n");
    _free(options);

    return true;
}

char *interfaceInput(char *header)
{
    char *response = malloc(sizeof(char) * 256);
    long size;

    if(response == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    infoScan("%s", header);
    fgets(response, 256, stdin);
    fflush(stdin); // clean stdin

    size = strlen(response);

    if(response[size - 1] == '\n') {
        response[size - 1] = '\0';
    }

    return response;
}

void _free(Options *options)
{
    free(options->title);
    free(options->label);
}

void _trim(char *value)
{
    int wordSize = strlen(value);
    char lastCharacter = value[wordSize - 1];

    if(lastCharacter == 0x20 || lastCharacter == 0x09 || lastCharacter == 0x0A || lastCharacter == 0x0D || lastCharacter == 0x0B) {
        value[wordSize - 1] = '\0';
    }
}

int _getInteger(char *value)
{
    char *endptr;
    long index = strtol(value, &endptr, 10);

    if((errno == ERANGE && (index == LONG_MIN || index == LONG_MAX))
        || (errno != 0 && index == 0)) {
        return -1;
    } else if(endptr == value) {
        return -1;
    }

    return index;
}

void _garbageCollector(char ***element, int counter)
{
    int index = 0;

    for( ; index < counter; index++) {
        free(element[counter]);
    }

    free(element);
    free(*element);
}
