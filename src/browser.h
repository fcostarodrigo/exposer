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

#ifndef BROWSER_H
#define BROWSER_H

#ifdef _WIN32
#define BROWSER_COMMAND "start /B http://127.0.0.1:%s"
#define BROWSER_COMMAND_N 31
#endif

#ifdef __APPLE__
#define BROWSER_COMMAND "open http://127.0.0.1:%s"
#define BROWSER_COMMAND_N 27
#endif

#ifndef BROWSER_COMMAND
#define BROWSER_COMMAND "xdg-open http://127.0.0.1:%s"
#define BROWSER_COMMAND_N 31
#endif

int browserOpen(char *port);

#endif
