#!/bin/bash

#rm -f  hello.sh hello

cat testEmbed.sh > hello.sh

gcc -o hello hello.c

uuencode hello hello >> hello.sh

rm -f hello
