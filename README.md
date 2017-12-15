# jerminus

A libxante application interpreter.

## License

LGPLv2

## Overview

jerminus is a interpreter to run applications which uses libxante as its core.

## Call an application

An application developed using libxante can be executed with jerminus with the
following command line:

```
$ jerminus -j application.jtf
```

For more details and options use the **-h** option.

## Compile

In order to compile the project you must have at least the following programs
installed;

* C compiler
* CMake, at least version 2.8

After installing them you can compile the project using the commands:

* mkdir build
* cd build
* cmake ..
* make

## Dependencies

* [libxante](https://github.com/rsfreitas/libxante)
* [libcollections](https://github.com/rsfreitas/libcollections)

