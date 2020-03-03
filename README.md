# HI-TECH-Z80-C
HI-TECH Z80 C Compiler for CP/M
===============================

In this repository I intend to apply updates and enhancements to
the HITECH C Compiler for Z80 running natively under CP/M.  Each
release will be a milestone with various updates and patches applied.

The first release is a slightly modified (see below) V3.09.

Background
----------

The HI-TECH Z80 CP/M C compiler V3.09 is provided free of charge for any
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

I've edited the documentation to remove ASCII underlining by over-printing
and fixed some layout issues and typos.

