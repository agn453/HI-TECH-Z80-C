			** Overlays with Hi-Tech C **

	The following procedure may be used to create overlays for use
with Hi-Tech C. Although messy to compile and link, it results in a smaller
TPA being used by a given program. Thus larger programs can be created: the
only penalty is the time taken to load the overlay from disk.

1. Creating the source files.
	An overlay can be called from the main program using

	[value = ]ovloader(filename,argument);

where <filename> is a string containing the overlay's filename (without the
.ovr extension) and <argument> is a single argument to the function. The
result of the overlay may optionally be assigned to a value, as in any C
function.

	The overlay may reference functions, global symbols etc. in the main
program, but otherwise appears as a normal C program, except that the main
function must be called ovmain() instead of main().

2. Linking and producing the overlays.

	The main function must be linked first. Ensure that the modules
ovloader.c and ovbgn.as (in that order) are linked as well, and request the
compiler to produce a symbol table (-F option).
	Once this has been completed, run the SYM2AS program on the .SYM
file (e.g. SYM2AS TEST.SYM), and this will produce a file named MAIN.AS,
containing the addresses from the main portion in assembler-source form.
Then run ZAS on this (ZAS MAIN.AS), and this will produce a file named
MAIN.OBJ. You will need this to link the overlays.
	You will need to examine the MAIN.AS file, as you need the address
of _ovbgn for the following step. Assume this is 01234h for the purposes
of this explanation.
	Compile the overlay(s) with the -C option to result in .OBJ files.
Do not invoke the linker as this will result in many errors! When you have
done this, make up a small ascii file (with wordstar etc.) to direct the
linker. The format of this file should be as follows (NOTE: MUST be in lower
case!):

-c01234h -ptext=01234h,data -otest1.ovr test1.obj <other modules as reqd> \
main.obj 0:a:libc.lib

(assuming the overlay is to be test1.ovr, the source was test1.c). Other
modules may be linked in as well. Note the use of the \ character to
continue input onto the next line. Naturally, you should replace the '01234h'
with the actual value of _ovbgn as obtained from the MAIN.AS file.

	Assume this file is named TEST.LNK. You are now ready to link the
overlay. Use the command

LINK <TEST.LNK

	and it should rumble away to itself and complete. Note that you do
not have to repeat the linking and loading of the main segment if you only
change the overlay source: the same main.obj file may be used each time the
overlay is re-linked. The TEST.LNK file should remain the same as well, but
remember to alter it as necessary if you change the main program file.

	The OVBGN.AS file contains an equate to set the maximum overlay
size: it is currently set to 5000h (20k). This should be sufficient for
most purposes, but it may be lowered if your overlays are small. If you link
the overlays with the -Mxxxx.MAP option in the .LNK file, a map of the
overlay will be produced. Examination of this will show whether the max
overlay size needs to be changed.

	Although messy, this process produces working overlays with Hi-Tech
C. This compiler does seem to produce small, fast code for Z80 machines,
and is probably worth persevering with.

			-- Ron Murray, 25/8/88


