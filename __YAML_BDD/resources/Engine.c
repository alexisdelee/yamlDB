#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "toolbox.h"
#include "colorShell.h"
#include "Engine.h"

Algo authorizedOperators(char *);
unsigned char engine(Algo, double, double);
double str2d(char *);
int eval(char *, char *, char *, int);

Engine engineInit()
{
    Engine engine;

    engine.eval = eval;

    return engine;
}

Algo authorizedOperators(char *operator)
{
    Algo algo;
    algo.equal = algo.lower = algo.upper = 0;
    algo.authorized = 1;

    if(!strcmp(operator, "=")) {
        algo.equal = 1;
    } else if(!strcmp(operator, "<>")) {
        algo.lower = algo.upper = 1;
    } else if(!strcmp(operator, "<")) {
        algo.lower = 1;
    } else if(!strcmp(operator, ">")) {
        algo.upper = 1;
    } else if(!strcmp(operator, "<=")) {
        algo.lower = algo.equal = 1;
    } else if(!strcmp(operator, ">=")) {
        algo.equal = algo.upper = 1;
    } else {
        algo.authorized = 0;
    }

    return algo;
}

unsigned char engine(Algo algo, double valueA, double valueB)
{
    unsigned char flag = 0;

    if(algo.lower) {
        flag |= valueA < valueB;
    }

    if(algo.upper) {
        flag |= valueA > valueB;
    }

    if(algo.equal) {
        flag |= valueA == valueB;
    }

    return flag;
}

double str2d(char *value)
{
    char *endptr = NULL;
    double real;

    real = strtod(value, &endptr);

    if((errno == ERANGE && (real == HUGE_VAL || real == -HUGE_VAL))
       || (errno && !real)
       || (endptr == value)) {
        return -HUGE_VAL;
    } else {
        return real;
    }
}

int eval(char *operator, char *valueA, char *valueB, int type)
{
    Algo authorization = authorizedOperators(operator);
    double a, b;
    int status;

    if(authorization.authorized) { // opérateur reconnu
        if(type & (YAML_CHARACTER | YAML_STRING)) { // type reconnu
            status = engine(authorization, (double)strcmp(valueA, valueB), (double)strcmp(valueB, valueA));
        } else {
            if((a = str2d(valueA)) == -HUGE_VAL || (b = str2d(valueB)) == -HUGE_VAL) { // échec convertion
                danger(false, "Exception: bad type\n");
                return false;
            } else {
                status = engine(authorization, a, b);
            }
        }

        if(!status) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}
