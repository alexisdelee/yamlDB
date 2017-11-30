#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "toolbox.h"
#include "colorShell.h"
#include "throw.h"

Throw *_setError(char *file, int line, char *format, ...)
{
    Throw *err = malloc(sizeof(Throw));
    if(err == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    va_list arglist;

    err->err = true;
    err->line = line;
    sprintf(err->file, "%s", file);

    va_start(arglist, format);
    vsprintf(err->message, format, arglist);
    va_end(arglist);

    return err;
}

Throw *_setSuccess(char *file, int line, char *format, ...)
{
    Throw *err = malloc(sizeof(Throw));
    if(err == NULL) {
        danger(true, "Exception: error with malloc\n");
    }

    va_list arglist;

    err->err = false;
    err->line = line;
    sprintf(err->file, "%s", file);

    va_start(arglist, format);
    vsprintf(err->message, format, arglist);
    va_end(arglist);

    return err;
}

void printStackError(Throw *err, int debug)
{
    if(err->err) {
        if(debug) {
            danger(false, "%s\n%2sat %s:%d\n", err->message, "", err->file, err->line);
        } else {
            danger(false, "%s\n", err->message);
        }
    } else {
        success("%s\n", err->message);
    }
}
