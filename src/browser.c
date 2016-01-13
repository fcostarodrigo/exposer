/*
    Copyright 2015 Rodrigo Fernandes da Costa

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Author: Rodrigo Fernandes da Costa

    E-mail: rodrigo5244@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>

#include "browser.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int browserOpen(char *port) {
    char command[BROWSER_COMMAND_N];

    sprintf(command, BROWSER_COMMAND, port);

    if (system(command)) {
        fprintf(stderr, "browser> Browser failed to start.\n");
        return 0;
    }

    if (DEBUG) {
        printf("browser> Browser started.\n");
    }

    return 1;
}
