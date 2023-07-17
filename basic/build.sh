#!/usr/bin/env bash

asm6809 -S -o srec-2.srec -l srec-2.lst srec-2.asm

./srec2bas.pl srec-2.srec >runme.txt
