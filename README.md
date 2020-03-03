HI-TECH Z80 C Compiler for CP/M
===============================

In this repository I intend to apply updates and enhancements to
the HITECH C Compiler for Z80 running natively under CP/M.  Each
release will be a milestone with various updates and patches applied.

The first release is a slightly modified (see below) v3.09.

Background
----------

The HI-TECH Z80 CP/M C compiler v3.09 is provided free of charge for any
use, private or commercial, strictly as-is. No warranty or product
support is offered or implied.

You may use this software for whatever you like, providing you acknowledge
that the copyright to this software remains with HI-TECH Software.

The *dist* folder contains the entire compiler including the library
source code in Huffman-encoded archive files (CPM.HUF, GEN.HUF,
FLOAT.HUF and STDIO.HUF).

The *cpm*, *gen*, *float* and *stdio* folders contain the library
source code that has been extracted from the .HUF archive files
using DEHUFF.COM.

The *doc* folder contains the documentation in ASCII text as HTCZ80.TXT.


Release v3.09
-------------

The original HITECH C linker (LINK.COM) has been renamed as LINQ.COM
and the main compiler front-end (C.COM) has been patched to reflect
this name change.  This was done to avoid a name clash with the Digital
Research supplied CP/M linker.  The original version is kept as C-ORIG.COM

I've edited the documentation to remove the underlining and bolding of
text by over-printing and fixed some layout issues and typos.


Release v3.09-1
---------------

As supplied in v3.09, the LIBF.LIB produces inaccurate floating point
trigonometry results - as reported by "Ed" in his posting to the
comp.os.cpm USENET Newsgroup.

```
Since we're on the topic of Hitech-C for CP/M, below is a problem I
discovered with the freeware release.

LIBF.LIB supplied with the freeware Hitech-C v3.09 produces inaccurate trig
results.  I found that by simply recompiling the library with the existing
source files from FLOAT.HUF, it fixed the problem.  Absolutely no changes
to the sources were required.
```

The re-compiled LIBF.LIB from the FLOAT.HUF library sources in *float*
has been copied to the distribution files in *dist*.  A SUBmit file to
rebuild LIBF.LIB has been added to *float*.

A *test* folder has been created.  In it is a TESTTRIG.C program for testing
this release.


Release v3.09-2
---------------

The late Jon Saxton (former SYSOP of the Australian Tesseract RCPM)
collected together a number of patches and updates - See Tesseract
volume 091.

These include contributions from John Elliot via
http://www.seasip.info/Cpm/software/index.html - in particular
for PIPEMGR which is a CP/M Plus RSX that implements piping and redirection.

John's contributions will be implemented in a later release.

This release includes updates to memset(), bios() and bdos() library
routines.  In addition the stat(), close() and sys_err() routines
are updated to remove references to bdoshl() and to include CP/M Plus
error messages.

```
memset()

     The Hi-Tech C implementation of memset() is seriously broken and is
     almost guaranteed to give trouble in any program which uses it.

     The implementation simply does not agree with the function declaration.

bios()

    The original bios() routine used the function number to construct an offset
    into the BIOS jump vector and from that calculated the address of the
    appropriate BDOS routine.  Except for some character I/O routines, that
    method of calling the BIOS is guaranteed to crash CP/M 3.

    A new bios() function has been implemented which checks for CP/M 3 and if
    that is the current operating system then it accesses the BIOS via the
    sanctioned method, i.e. by invoking bdos(50) with an appropriately filled-
    out parameter block.

    The standard bios() routine takes 1, 2 or 3 parameters:

        short bios(int func, int bc, int de)

    and that is good enough for CP/M 2.2.  However CP/M 3 has functions which
    also take inputs in A and/or HL.  The routine is smart enough to figure
    out where to put the parameters.  All the C programmer needs to do is to
    supply the requisite arguments in the right order.

    A bug in the 2.2 bios() routine has been fixed.  The original code would
    always return an 8-bit result even though there are some functions which
    return 16-bit results.

bdos()

    All versions of CP/M 80 return from BDOS calls with BA = HL regardless of
    whether the function is returning an 8-bit or 16-bit result.  Hi-Tech C
    provided a bdos() call for 8-bit results and a bdoshl() call for 16-bit
    results.  John Elliott added a third, bdose(), for disk functions which
    could return extended error codes in H (and B) when running on CP/M 3.

    There is now a single bdos() function.  It always returns a 16-bit result
    in HL.  The bdoshl() routine still exists for compatibility with older
    source files but it is simply an alias for bdos().  John Elliott's
    bdose() routine no longer exists; bdos() also performs its functions.

    Some BDOS functions return 255 as an error flag.  The old bdos() code
    would sign-extend that to -1 (16 bit) but that is no longer done.

        if (bdos(fn, data) == -1)               /* This won't work */

        if (bdos(fn, data) == 255)              /* This works sometimes */

        if ((bdos(fn, data) & 0xFF) == 255)     /* This always works */

    Under CP/M Plus several functions return extended error information in
    the upper 8 bits of the return value.  (Those functions also set the
    standard errno global item.)  Always use the third form when checking
    for an error result.  The compiler is quite clever and doesn't make your
    program work harder ... it just uses the L register.
```

