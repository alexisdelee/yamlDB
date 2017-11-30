#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toolbox.h"
#include "colorShell.h"
#include "settings.h"

void _settingsParse(Settings *, char *);
Settings defaultSettings();

Settings getSettings()
{
    FILE *conf = NULL;
    Settings settings = defaultSettings();
    char line[1001];
    int size;

    if((conf = fopen("yaml.conf", "r")) == false) {
        danger(true, "Exception: non-existent configuration file (yaml.conf)\n");
    }

    while(fgets(line, 1001, conf)) {
        size = strlen(line);
        if(line[size - 1] == '\n') {
            line[size - 1] = '\0';
        }

        _settingsParse(&settings, line);
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
    settings.debug = false;

    return settings;
}

void _settingsParse(Settings *settings, char *data)
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
    } else if(!strcmp(_key, "debug") && !strcmp(_value, "true")) {
        settings->debug = true;
    }
}
