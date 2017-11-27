#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toolbox.h"
#include "colorShell.h"
#include "global.h"

void _settingsParse(Config *, char *, char *);
Settings defaultSettings();

Settings getSettings(int argc, char **argv)
{
    FILE *conf = NULL;
    Settings settings = defaultSettings();
    char line[1001];
    int size;

    if(argc < 2) {
        fclose(conf);
        danger(true, "YAML_BDD <config path>\n");
    } else {
        if((conf = fopen(argv[1], "r")) == false) {
            danger(true, "Exception: non-existent configuration file\n");
        }
    }

    while(fgets(line, 1001, conf)) {
        size = strlen(line);
        if(line[size - 1] == '\n') {
            line[size - 1] = '\0';
        }

        _settingsParse(&settings, line, "environment");
    }

    fclose(conf);
    return settings;
}

Settings defaultSettings()
{
    Settings settings;

    strcpy(settings.environment, "");
    settings.allowColor = false;
    settings.tree = false;

    return settings;
}

void _settingsParse(Settings *settings, char *data, char *key)
{
    char _key[256];
    char _value[256];
    int index;

    if((index = strcspn(data, "=")) == strlen(data)) {
        return;
    }

    sprintf(_key, "%.*s", index, data);
    sprintf(_value, "%s", data + index + 1);

    if(!strcmp(_key, "environment")) {
        sprintf(settings->environment, "%s", _value);
    } else if(!strcmp(_key, "allow-color") && !strcmp(_value, "true")) {
        settings->allowColor = true;
    } else if(!strcmp(_key, "tree") && !strcmp(_value, "true")) {
        settings->tree = true;
    }
}
