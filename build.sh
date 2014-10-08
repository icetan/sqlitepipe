#!/bin/sh

gcc -bundle -fPIC -Isqlite3 -o pipe.sqlext sqlitepipe.c
