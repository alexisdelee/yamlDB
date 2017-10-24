#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "colorShell.h"

void welcome(char *format, ...)
{
    va_list arglist;

    va_start(arglist, format);

    if(ansiSupport) {
        printf(ANSI_COLOR_CYAN "[*] " ANSI_COLOR_YELLOW);
        vprintf(format, arglist);
        printf(ANSI_COLOR_RESET);
    } else {
        printf("[*] ");
        vprintf(format, arglist);
    }

    va_end(arglist);
}

void warningList(char *format, ...)
{
    va_list arglist;
    int index = 0;
    char secureString[501]; // maximum 500 characters

    if((index = strcspn(format, "]")) == strlen(format)) {
        index = 0;
    }

    va_start(arglist, format);

    if(ansiSupport) {
        vsprintf(secureString, format, arglist);

        printf(ANSI_COLOR_CYAN "%.*s" ANSI_COLOR_YELLOW, index, secureString);
        printf("%s" ANSI_COLOR_RESET, secureString + index);
    } else {
        vprintf(format, arglist);
    }

    va_end(arglist);
}

void infoScan(char *format, ...)
{
    va_list arglist;

    va_start(arglist, format);

    if(ansiSupport) {
        printf(ANSI_COLOR_GREEN);
        vprintf(format, arglist);
        printf(ANSI_COLOR_RESET);
    } else {
        vprintf(format, arglist);
    }

    va_end(arglist);
}

void danger(int quitMode, char *format, ...)
{
    va_list arglist;

    va_start(arglist, format);

    if(ansiSupport) {
        printf(ANSI_COLOR_RED);
        vprintf(format, arglist);
        printf(ANSI_COLOR_RESET);
    } else {
        vprintf(format, arglist);
    }

    va_end(arglist);

    if(quitMode) {
        exit(-1);
    }
}

void success(char *format, ...)
{
    va_list arglist;

    va_start(arglist, format);

    if(ansiSupport) {
        printf(ANSI_COLOR_MAGENTA);
        vprintf(format, arglist);
        printf(ANSI_COLOR_RESET);
    } else {
        vprintf(format, arglist);
    }

    va_end(arglist);
}
