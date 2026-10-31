LIB9511 is the Am9511A math library for Hi-Tech Z80 C on the RC2014 APU Module.

The library replaces the float kernel and functions that the Am9511A APU calculates.

Link LIB9511.LIB before LIBF.LIB.
LINQ then takes the Am9511A module functions and leaves the rest of LIBF in the program.

## Build

Log on to the drive that holds these sources.
Submit MAKELIB.SUB.
The submit file runs ZAS on each .AS file and then builds LIB9511.LIB with LIBR.

A host with the Hi-Tech cross tools can run make in this directory.
The MAKEFILE calls zas and libr.

## Use

Include math.h for the standard names.
Include am9511.h for pi, log2, exp2, and exp10.
The header am9511.h includes math.h.
On the RC2014, pass -II: when AM9511.H is on drive I:.

A program that stays under 128 records can link from the C command.

C -V -IH: -II: PROG.C LIB9511.LIB LIBF.LIB

Name LIB9511.LIB first and LIBF.LIB second.
The driver adds LIBC.LIB.
The product is a COM file on the logged drive.

A longer program (over 128 records) needs three steps.
C -C writes the object and does not link.
Write the LINQ response with a line feed only.
The CCP folds a typed line to upper case and would change -Ptext=0,data,bss.

-Z -Ptext=0,data,bss -OPROG.OBJ CRTCPM.OBJ PROG.OBJ LIB9511.LIB LIBF.LIB LIBC.LIB

OBJTOHEX -B100H PROG.OBJ PROG.COM

The data port is 42h.
The command port is 43h.
Overflow, underflow, divide by zero, and a domain error return 0.
The chip truncates a float toward zero when it converts to a long.
A magnitude that does not fit in a signed long returns 0.
floor and ceil use the exponent field, so a large value stays correct.
