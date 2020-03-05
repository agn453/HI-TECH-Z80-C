




       1. Debugger Reference Manual


            HI-TECH C is supplied  with  an  interactive  debugging
       tool  oriented toward C programs. It is not a "source level"
       debugger, i.e. it has no knowledge of  the  C  source  code,
       however it does have the facility to handle C symbols and to
       show the C function calling sequence.

            The command structure is modelled on that of  the  Unix
       debugger  known  as  adb.  It provides facilities to display
       memory in various radices or as  instructions,  to  set  and
       remove  breakpoints,  which may have a repeat count and/or a
       command associated with them. It is possible to set a break-
       point which will stop only if a certain condition is true.

            The debugger may be used on any .COM file,  however  in
       order  to  take  advantage of the symbolic facilities, it is
       necessary to generate a symbol file,  usually  with  the  -F
       option  to the C command. This file consists of one line per
       symbol, with the hexadecimal value of the  symbol  preceding
       the symbol on the line.

       1.1. Invoking Debug

            The debugger is invoked by the command  DEBUG.  It  may
       have  zero, one or two file arguments. The first argument is
       the name of a file in .COM format to be be debugged, and the
       second  a symbol file name. If the symbol file name is omit-
       ted, no symbols will be available. If the .COM  format  file
       is omitted, no code will be loaded. Some examples:

            DEBUG fred.com
            DEBUG bill.com l.sym


       1.2. Run Time Organization

            DEBUG relocates itself below the  BDOS  when  executed,
       allowing  the  debugee to be loaded at the start of the TPA,
       as usual. The symbol table, if loaded, grows downwards  from
       the  base of the relocated debugger. The BDOS entry at loca-
       tion 5 is changed to reflect the base of  the  symbol  table
       rather  than the base of the BDOS. Thus the symbol table and
       the debugger are not owerwritten by the debugee's stack.

            Breakpoints are inserted in the code as RST 8  instruc-
       tions. A jump is placed at location 8 to the debugger's trap
       handler. This is unfortunate if your system happens  to  use
       RST  8  for  interrupts, but this is probably no more likely
       than that it uses any other restart location.











   Page 2         HI-TECH C (Z80) USER'S MANUAL


   1.3. Commands

        The basic debugger command syntax is:

        address , count command modifier extrastuff


        This may seem a little obscure, so read on. Address and
   count are expressions, in their simplest form simply hexade-
   cimal numbers. Both address and count are optional,  but  if
   count  is  to  be  specified with no address, the comma must
   appear.

        Command is a single character specifying what the  com-
   mand  should  do. Modifier is another character which deter-
   mines more specifically what the command is.  Extrastuff  is
   dependent on the particular command, and is usually omitted.

   1.3.1. Expressions

        Expressions may consist of:

   .    The value of the current location (not necessarily  the
        current  PC  or  last  breakpoint,  this is an internal
        current value).

   SYMBOL
        The value of a symbol,  as  looked  up  in  the  symbol
        table.   If  the  symbol  is not found, the same symbol
        prepended by and underscore will be  looked  for.  This
        allows  C symbols to be referred to without the leading
        underscore tacked on by the compiler.

   INTEGER
        A hexadecimal integer. It must start with a digit, oth-
        erwise the debugger will think it is a symbol.

   <REGNAME
        This yields the contents of the specified Z80 register.
        The  register  names  are the usual Z80 names, in lower
        case only. See the $r command below.

   (EXPR)
        Parentheses may be uses to enclose expressions to force
        evaluation order.

   *EXPR
        The contents of the  word  at  address  EXPR.  This  is
        indirection analogous to the C indirection operator.

   -EXPR
        The negation of EXPR.











                      HI-TECH C (Z80) USER'S MANUAL          Page 3


       ~EXPR
            The bitwise complement of EXPR.

       E1+E2
            The sum of E1 and E2.

       E1-E2
            The value of E1 less the value of E2.

       E1*E2
            The value of E1 multiplied by E2.

       E1%E2
            E1 divided by E2.

       E1&E2
            E1 anded with E2.

            The usual precedence relationships  apply;  parentheses
       may be used to alter the order of evaluation.

       1.3.2. Command Characters

            The main command character used is /. This is  used  to
       display  memory  in  various radices or as instructions. The
       exact format is determined by the modifier character, or the
       previous  format  used if the modifier character is omitted.
       The modifiers are:

       i    Print as Z80 instructions

       h or b
            Print as hexadecimal bytes.

       o    Print as octal bytes.

       d    Print as decimal bytes.

       H or W
            Print as hexadecimal words.

       O    Octal words.

       D    Decimal words.

       c    Print each byte as an ascii character.

       C    Print as ascii characters if printable, as @x  if  not,
            where x is the corresponding alphabetic character, e.g.
            @C for 3.

       s    Print a string of characters up to a null.











   Page 4         HI-TECH C (Z80) USER'S MANUAL


   a    Print the address as a symbolic value.

        If address is specified, the display  starts  from  the
   specified address. Count bytes, instructions, words, strings
   or whatever will be printed, or 1 if count is  omitted.  For
   example:

        fred,10/b
        123/i

   The first command prints 16 bytes from the  address  of  the
   symbol fred.  The second displays one instruction from loca-
   tion 123H.

        When such a command is issued, the value of dot is tem-
   porarily incremented by the total number of bytes displayed.
   A subsequent command consisting solely of a RETURN  or  LINE
   FEED will make the temporary increment of dot permanent, and
   execute a / command. Thus RETURN may be used to  step  along
   in  memory, displaying memory in the same format as the last
   / command.

        The / command may also be used to alter memory. /w EXPR
   will  write  the  value  of  expr into memory at the current
   location (dot). Either a word or  a  byte  will  be  written
   depending on the last format used for a / command. A /w com-
   mand may not be issued if the current format is not  a  byte
   or  word type. Thus memory may not be modified in the i for-
   mat. One day there will be an in-line  assembler  built  in,
   but not yet.

        The ] command is like the /  command,  except  that  it
   displays  I/O  ports rather than memory. It may be used with
   only the h, b, o or d formats.  In addition, ]w may be  used
   to write to an I/O port.

        The $ command has various modifiers as described below:

   c    Print a C stack backtrace. Note that this is not  reli-
        able when used on an optimized program since the optim-
        izer changes stack manipulation code. There may  appear
        to be fewer arguments than there really are. Long argu-
        ments will always appear as two integer arguments.

   b    Display currently set breakpoints

   s    Set the limit for symbol matches to the given  address.
        This determines the maximum value of offset when print-
        ing out a value as sym+offset.

   w    Set the terminal width to address. The  default  is  80
        decimal.











                      HI-TECH C (Z80) USER'S MANUAL          Page 5


       r    Display the contents of all Z80 registers.

            The : command has the following modifiers:

       r    Run the program from address. If  address  is  omitted,
            try  for  the  symbol  start.  If  it is not found, the
            debugger will complain. With this  command,  extrastuff
            will  be supplied as an argument string to the program,
            i.e. in the default buffer at 80H.  You  should  ensure
            there is a space between the r and extrastuff. E.g.:

                        100:r arg1 arg2

       c    Continue the program from address, or the  contents  of
            PC if address is omitted. Used after a breakpoint.

       s    As for c, but execute only count instructions, or 1  if
            count is omitted. Thus this single steps the debugee.

       b    Set a breakpoint at address. If count is supplied,  the
            breakpoint  will  not  stop until it has been hit count
            times. Extrastuff may be a  command  to  execute  every
            time  the breakpoint is hit. If the command sets dot to
            zero, then the breakpoint will stop even  if  count  is
            not zero.

       .    Set a temporary breakpoint at address and continue exe-
            cution.   When the next breakpoint is encountered, this
            temporary breakpoint will be removed.

       d    Clear the breakpoint at address.

            The command > allows the  values  of  registers  to  be
       changed.  Both word and byte registers may be specified. The
       interrupt flag may also be changed.  0 means  off,  1  means
       on.

       1.4. Example

            An example of the use of the debugger follows:



       A>type tst.c
       main()
       {
           int     i, j;

           scanf("%d", &i);
           printf("%d\n", j); Note the error - j should be i
       }

       A>c -f tst.c      Compile requesting a symbol file










   Page 6         HI-TECH C (Z80) USER'S MANUAL


   A>debug tst.com l.sym Default symbol file name is l.sym
   ZDEBUG
   : printf/i     Disassemble at printf
   _printf:        call    csv
   :              Step down with RETURN
   _printf+3:      push    ix
   : :b           Set a breakpoint here
   : :r           Run the program - no arguments
   123             Scanf waits for input - enter 123
   Breakpoint      _printf+3
   _printf+3:      push    ix  Stopped at the breakpoint
   : $c           Get a stack trace
   _printf(1872,0)      1872 is the format string "d\n"
   _main()
   : main/i       Look at main() now
   _main:          call    csv
   :              Step down with RETURN
   _main+3:        ld      hl,FFFC
   :
   _main+6:        add     hl,sp
   : ,10          Disassemble 16 instructions
   _main+7:        ld      sp,hl
   _main+8:        push    ix
   _main+A:        pop     hl
   _main+B:        dec     hl
   _main+C:        dec     hl
   _main+D:        push    hl
   _main+E:        ld      hl,186F
   _main+11:       push    hl
   _main+12:       call    _scanf
   _main+15:       ld      hl,4
   _main+18:       add     hl,sp
   _main+19:       ld      sp,hl
   _main+1A:       ld      l,(ix+-4) here j is loaded
   _main+1D:       ld      h,(ix+-3)
   _main+20:       push    hl    And pushed onto the stack
   _main+21:       ld      hl,1872
   : main+1a/i
   _main+1A:       ld      l,(ix+-4)
   : /h           Look at the bytes as hex
   _main+1A:       DD        Indexing prefix byte
   :              Step down with RETURN
   _main+1B:       6E
   :
   _main+1C:       FC        The index offset = -4
   : /w 0fe       Change to -2
   :
   _main+1D:       DD
   :
   _main+1E:       66
   :
   _main+1F:       FD
   : /w 0ff   Change the hi byte to -1 to address i instead of j
   : :r       Run it again









                      HI-TECH C (Z80) USER'S MANUAL          Page 7


       123        Enter the same number again
       Breakpoint      _printf+3
       _printf+3:      push    ix
       : $c
       _printf(1872,7B)
       _main()
       : 7b=d        7b was the argument above
        123          Now we have the correct value
       : :c        Continue the program
       123          Which prints the correct value

       A>
















































