#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"

void analyzerArguments(int argc, char **argv)
{
    int i, size;

    if(argc < 2) {
        printf("Exception: this program takes as parameter the path to the folder containing the databases\n\n");
        printf("__YAML_BDD.exe databases_path<string> [--color-mode][--statistic-mode]\n");

        exit(-1);
	} else {
		for(i = 1; i < argc; i++) {
			switch(i) {
				case 1:
					size = strlen(argv[1]);

					if(size > 255) {
						printf("Exception: the parent folder contains more than the number of characters allowed (255 max)\n");
						exit(-1);
					}

					sprintf(container, "%s", argv[1]);

					if(container[size - 1] != '/') {
						strcat(container, "/");
					}

					break;
				default:
					if(!strcmp(argv[i], "--color-mode")) {
						colorModeOption = 1;
					} else if(!strcmp(argv[i], "--statistic-mode")) {
						statisticModeOption = 1;
					}
			}
		}
	}
}
