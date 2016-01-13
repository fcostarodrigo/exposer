# Exposer

Author: Rodrigo Fernandes da Costa

E-mail: rodrigo5244@gmail.com

## Overview

Web apps offer many features of native apps, but unfortunately they still don't provide everything that is expected from a native app. This program allows you to develop a web app that runs in the local machine and have access to all native apps features.

To accomplish this, the program run as a tiny web server that only serves in the host. After you run the program it lauches the default web browser pointed to the web app located in the same folder as the application. Regular native apps features are provided to the web app by a HTTP API including the command line arguments. This way when you click the program or open a file with the program the browser is lauched with a page that can access the file and do things that regular native apps can do. Another way of looking at Exposer is to look at it as a regular native app that uses the default browser to draw its GUI.

## Usage

Put an index.html file in the same folder as the executable. In that file you can access the API by doing HTTP resquests with specific paths.

Each path correspond to a function, the arguments are passed via HTTPs request headers named arg0, arg1, and so on.

Check the [complete api list](http://rodrigofcosta.net/#About/Exposer/API).

If you run the application via the command line you can get access to all the trafict exchanged between the web app and the server.

## Compilation

In GNU + Linux

    gcc *.c -lm

In Windows with MinGW

    gcc *.c -mwindows -lwsock32 -lws2_32

The option `-mwindows` disables the console.
