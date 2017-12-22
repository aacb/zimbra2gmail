#!/bin/bash

gcc -c -o lex.o lex.c
gcc -c -o global.o global.c
gcc -c -o main.o main.c
gcc -o zimbra2gmail main.o global.o lex.o

rm -f main.o global.o lex.o

