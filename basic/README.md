# srec-run.bas

This is a BBC BASIC program which can load Motorola SREC records to memory
and optionally execute them should an S9 record be present.

It is intended for short programs that need to be run from a BBC BASIC 
environment for longer programs the srec-2 program below may be more efficient

# srec-2.asm

This program is first assembled to an SREC object and then converted to a set
of BBC BASIC pokes. The pokes load the program at &E00 and run it. The srec-2 
program then relocates itself to &800 to free up more space and can be used to
load/run programs at &E00

It is intended that the srec-2.bas program be prepended to a longer SREC 
program to make a longer self-bootstrapping program that can be loaded and
executed from BASIC