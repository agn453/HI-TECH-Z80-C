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
for PIPEMGR which is a CP/M 3 RSX that implements piping and redirection.

John's contributions will be implemented in a later release.

This release includes updates to memset(), bios() and bdos() library
routines.  In addition the stat(), close() and sys_err() routines
are updated to remove references to bdoshl() and to include CP/M 3
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

    Under CP/M 3 several functions return extended error information in
    the upper 8 bits of the return value.  (Those functions also set the
    standard errno global item.)  Always use the third form when checking
    for an error result.  The compiler is quite clever and doesn't make your
    program work harder ... it just uses the L register.
```


Release v3.09-3
---------------

These are the Job Saxton modified versions of John Elliots's patches.

```
Wildcard expansion

    Programs compiled with this update get wildcard expansion of the CP/M
    command line automatically.  There is no longer any need to call
    _getargs() explicitly.

    Enclosing an argument in quote marks (' or ") supresses expansion.
    This can be useful for programs like grep which may use ? and/or * in
    text search patterns or for program options containing a question mark:

        grep 'a.*end' *.h 2d:*.c
        grep "-?"

    The -R option passed to the Hi-Tech C compiler is no longer useful.  (It
    didn't work anyway.)

    Implementing automatic argument expansion meant altering the order of
    modules in LIBC.LIB.  That entailed rebuilding the entire library from
    scratch.  A script to do that is supplied.

Filename drive and user number prefixes

    The format of file name prefixes indicating drive letter and/or user
    number is now much more liberal.  If a file "sample.txt" is on drive
    E: in user area 12 then depending on the current drive/user the file
    may be accessed as:

        sample.txt              if current DU: is E12:
        12:sample.txt           if current disk is E:
        e:sample.txt            if current user is 12
        e12:sample.txt
        12e:sample.txt
        12:e:sample.txt         (Hi-Tech C format)

    Note that any of these forms is acceptable for program arguments, even
    those containng wildcard characters (?, *).

Exact file sizes (CP/M 3 and DOS+ v2.5)

    Exact file sizes are supported on CP/M 3.  Under CP/M 2.2 file sizes are
    always a multiple of 128 bytes but some versions of CP/M allow setting
    and reading a "last sector byte count" field in the FCB.  The concept was
    carried forward from CP/M's ancestor, ISIS, but DRI did not document the
    precise meaning of the count for CP/M so two distinct interpretations were
    possible:

    a.  LSBC represents the number of UNUSED bytes in the last sector;

    b.  LSBC contains the number of USED bytes but 0 means 128.

    Both usages have appeared in the small number of CP/M 3 utilities which
    support exact file lengths.  My own tools use the first interpretation;
    John Elliott's utilities use the second.

    The first interpretation is slightly simpler to implement because it
    avoids any special-case handling.  It is also historically correct.
    Neither of these considerations is particularly significant but we do need
    to have programs which work together and interpret the last sector byte
    count the same way.  After a lifetime in software development I have
    learned to avoid handling special cases as far as possible.  Accordingly
    I have modified John Elliott's routines to interpret the last sector byte
    count as the number of unused bytes in the last sector.

    With this interpretation the formula for calculating the number of bytes
    in a file is always (sectors * 128) - lsbc.

    (When running on CP/M 2 lsbc is always zero.)

strcasecmp()

    The case-insensitive string comparison function strcasecmp() has been
    implemented.  Its function prototype is in string.h.  It works just like
    strcmp() except that upper- and lower-case letters are treated as 
    identical.  There is also a strncasecmp() analogue of strncmp() which
    allows one to limit the comparison to a certain number of cbaracters.

toupper() and tolower()

    The functions toupper() and tolower() were implemented as macros which
    added a case-shift value to the character parameter without checking to
    see if the parameter was a letter.  To use these routines correctly it was
    necessary to do a range check, e.g.

        if (isupper(c))
            c = tolower(c);

    These operations now invoke the correspondingly named routines in LIBC.LIB
    and it is no longer necessary to pre-test the character.

Support for read/write files

    The stdio functions now allow files to be opened in modes "r+" and "w+" - 
    ie, reading and writing are supported on the same file.
    Remember to fflush() or fseek() between a read and a write.
    This code is experimental.

PIPEMGR support (CP/M 3)

    Programs automatically check for PIPEMGR when they load, and if it
    is present the C streams stdin, stdout and stderr are routed to their
    PIPEMGR counterparts - no modification of the source is required.

    The special device files (CON: LST: PUN: and RDR:) are joined by RSX: 
    (PIPEMGR stdin/stdout streams) and ERR: (PIPEMGR stderr stream). If
    PIPEMGR is not present, these files behave like CON:

    The variable:

        extern char _piped;

    is nonzero if PIPEMGR is present.

    The command 

        FOO <CON:

    will behave differently if PIPEMGR is present. If it is not present,
    then you get line-based input from the Hi-Tech C library. If PIPEMGR
    is present then you get character-based input from PIPEMGR.

    The line-based input in the library will now return EOF if a control-Z
    is typed on a line by itself. Previously it would never return EOF on 
    line-based input.

    Unfortunately if you're using Simeon Cran's replacement ZPM3 then it
    does not allow control-Z to be typed on an input line.
    I'm not yet sure how to get round this.
  
    Remember that when PIPEMGR has parsed the command line, you may have a 
    number of blank (zero-length) arguments.

CP/M 3 extended error support

    To enable CP/M 3 extended error support, put the line

        bdos(0x2D,0xFF);

    near the beginning of your program. Then errors such as read-only files 
    will not cause the program to terminate abruptly; instead the call which
    caused the error will fail, and perror() will explain the reason. The 
    variable:

        extern int errno;

    will contain (16+x) for CP/M 3 extended error x.

File passwords (CP/M 3)

    NB: This feature is disabled.  You will need to change the
        PWDREC definition in the file BDOS.AS to true and rebuild
        and replace the module in LIBC.LIB to enable it.

    There is partial support for file passwords. To enable it, declare a 
    function:

        #include <cpm.h>

        char *mypass(struct FCB *fcb)
        {
        /* For the file specified by "fcb", a password needs to be entered.
           Return a pointer to it (8 characters, uppercase, packed with
           spaces) 
         */
        }

    and insert a line near the beginning of your program:

        _passwd = mypass;

    Then your routine ("mypass" in this example) will be called when
    CP/M reports a password error; it will be called repeatedly until
    the user enters the correct password or a it returns a NULL pointer
    (ie, the user has given up).

Checks for a ZCPR3 environment

    The variable

        extern void *_z3env;

    is 0 if one is not present, or its address if one is. It also passes it
    as a third parameter to main():

        int main(int argc, char **argv, void *z3env);

    The Z3 environment address must be passed to the program in HL.

Graceful exits

    Compiled programs exit gracefully if run on an 8080 or 8086 processor,
    or if there is not enough memory for the program and its static data.
    This is done in the CRTCPM.OBJ module.

CP/M 3 compatible error system

    exit(0) and _exit(0) set the CP/M 3 error code to 0.
    exit(n) and _exit(n) for non-zero n set the error code to 0xFF00 | (n&0x7F).

    The practical upshot is that exit(0) translates as "OK"; other values 
    translate as "error". If the next command is preceded by a : character, it 
    will be ignored:

        CPROG 
        :OTHER

    will only execute OTHER if CPROG exited with the value 0.

Extended getenv()

    Under CP/M 3, getenv() uses the search path set by SETDEF to locate the
    ENVIRON file. So if you like to keep ENVIRON on a drive that isn't A:, 
    programs will still find it.
```


Release v3.09-3b
----------------

Updated the previous release to adjust the ordering of modules in LIBC.LIB -
programs should now link correctly.  Also added the missing documentation for
the debugger in doc/DEBUGMAN.TXT (this was previously available from the
now defunct hitech.com.au web site) and scanned PDFs of the 1989
vintage documentation.


Release v3.09-4
---------------

Some of the string related function prototypes in STRING.H are missing from
the LIBC.LIB library.  Most of these are from an update to the MSX flavour
of HI-TECH C by Arnold Metsalaar.  I've cherry-picked the following routines
which are applicable to generic CP/M as well as MSX.

```
STRING.H
    The header file has been changed to reflect the available
    functions in LIBC.LIB.  There are still missing routines -
    namely strcoll() strcspn() strpbrk() and strspn() and these
    have been commented out for now.

strchr() and strrchr()
        char *strchr(char *s, int c)
        char *strrchr(char *s, int c)
    These functions - as well as index() and rindex() (which are identical)
    previously returned a NULL for no match.  The functions now return
    a pointer to the character string's ending NUL character.

stricmp() and strnicmp()
        char stricmp(char *s1, char *s2)
        char strnicmp(char *s1, char *s2, size_t n)
    Case-insensitive versions of strcmp() and strncmp() comparison routines.
    Can also be referenced as strcasecmp() and strncasecmp().

strstr(), strnstr(), stristr() and strnistr()
        char *strstr(char *t, char *s)
        char *strnstr(char *t, char *s, unsigned int n)
        char *strcasestr(char *t, char *s)
        char *strncasestr(char *t, char *s, unsigned int n)
    These extra functions locate the first occurrence of string s in
    string t.  The functions strnstr() and strcasestr() read at most
    n characters from the string t.  The functions strcasestr() and
    casencasestr() use case insensitive comparisons.
    All these functions return a pointer to the first character of
    the first occurence ofstring s in string t if found, and NULL
    otherwise.

strdup()
        char *strdup(char *s)
    Allocates a new buffer for and copies the string pointed to
    by s to it.  Returns a pointer to the copy of the string or NULL
    if the memory allocation failed. The memory block can be released
    using free().

strtok()
        char *strtok(char *s, char *tok, size_t toklen, char *brk)
    Copies charactersfrom s to tok until it encounters one of the
    characters in brk or until toklen-1 characters have been copied
    (whichever comes first).  It then adds a NUL character to the
    end of the string.  This is a non-conforming POSIX function.

TIME.H
    Now includes a prototype for strftime() - see below.

strftime()
        size_t strftime(char *s, size_t maxs, char *f, struct tm *t)
    Converts a time value t to a string using the format string f
    into the string s of size maxs (including a terminating NUL).
    It acts as a sprintf() function for date/time values. The
    following are valid in the format string -

             %A      full weekday name (Monday)
             %a      abbreviated weekday name (Mon)
             %B      full month name (January)
             %b      abbreviated month name (Jan)
             %c      standard date and time representation
             %d      day-of-month (01-31)
             %H      hour (24 hour clock) (00-23)
             %I      hour (12 hour clock) (01-12)
             %j      day-of-year (001-366)
             %M      minute (00-59)
             %m      month (01-12)
             %p      local equivalent of AM or PM
             %S      second (00-59)
             %U      week-of-year, first day sunday (00-53)
             %W      week-of-year, first day monday (00-53)
             %w      weekday (0-6, sunday is 0)
             %X      standard time representation
             %x      standard date representation
             %Y      year with century
             %y      year without century (00-99)
             %Z      timezone name
             %%      percent sign

    the standard date string is equivalent to:

        %a %b %d %Y

    the standard time string is equivalent to:

        %H:%M:%S

    the standard date and time string is equivalent to:

        %a %b %d %H:%M:%S %Y

    strftime() returns the number of characters placed in the
    buffer, not including the terminating NUL, or zero if more
    than maxs characters were produced.
```
