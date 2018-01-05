#!/bin/bash

gcc -c -o sint.o sint.c
gcc -c -o lex.o lex.c
gcc -c -o global.o global.c
gcc -c -o main.o main.c
gcc -o zimbra2gmail main.o global.o lex.o sint.o

rm -f main.o global.o lex.o sint.o

