# LIB9511 work plan

This file is the work plan for an Am9511A float library for Hi-Tech Z80 C.
The library name is `LIB9511.LIB`.
The source will be ZAS assembly that the RC2014 can build.
This file is a plan only.
Do not commit it until a commit is requested.
Do not start a pull request from this file.

The target machine is the RC2014 Z80 with the Am9511A on the APU module.
The compiler set is Hi-Tech Z80 C V3.09-21.
The driver on drive H: is `C.COM`.
The linker program is `LINQ`.

## Decision

Stage 1 replaces one object module.
That module is the arithmetic kernel in `float/FLOAT.AS`.
`LIBF.LIB` keeps the C functions, formatted I/O, and the other assembly modules.
The link line names `LIB9511.LIB` before `LIBF.LIB`.
LINQ then uses the new kernel and the existing C code.

A second copy of every C file is not the first deliverable.
The chip can replace selected C modules later.
Each replacement must export every public of the `LIBF` module it replaces.

## Where the float format is written down

Hi-Tech Z80 `float` and `double` are not IEEE-754.
`long double` is the same 32-bit value.
The i8086 and m68k block in `dist/FLOAT.H` is IEEE.
That block is not this compiler.

The user's manual does not draw the bits.
`doc/HTCZ80.TXT` section 12.3.4.2 (`DEFF`) says that ZAS stores a 32-bit Hi-Tech constant.
It gives no byte layout.
This repo has no ZAS source for `DEFF`.
The `FREXP, LDEXP` page states the numeric split.
`frexp` returns a fraction in the interval [0.5, 1.0) and an integer power of two.
The page does not name a bias.

The bit picture is the comment at the top of `float/FLOAT.AS`.

- Bit 7 of the high byte is the sign.
- The next 7 bits are the exponent.
- The other three bytes are a normalised 24-bit mantissa.
- The mantissa sits in the low-order bytes.
- The sign is the most significant bit of the most significant byte.

That comment does not state the bias or the binary point.
Those come from the routines that consume the bits.

| File | What it shows |
|------|----------------|
| `float/FREXP.AS` | Subtract 64 to store the power. Rebuild the fraction with exponent field 64. |
| `float/LTOF.AS` | A 24-bit integer starts at exponent `64+24`. One extra shift uses `64+25`. |
| `float/FTOL.AS` | Subtract `64+24` before the shift back to an integer. |
| `float/FBCD.AS` | Same `64+24` offset for the decimal conversion. |
| `float/FLOAT.AS` | Multiply subtracts `41h` (65) when it removes the bias. |
| `dist/FLOAT.H` | Under `#if z80`: 24-bit mantissa, `FLT_MIN_EXP` -62, `FLT_MAX_EXP` 64. |

`dist/FLOAT.H` prints `FLT_EPSILON` and `DBL_EPSILON` as `-1.192093`.
`FLT_MAX` is printed as `1.84467e19`.
`FLT_MIN` is printed as `1.084202e-19`.
Leave those texts as they stand.
Do not edit `FLOAT.H` for this job.
The header `FLT_MAX` is 2^64.
A 7-bit field cannot store that binade.
The chip cannot store it either.

The device format is in the z88dk file `libsrc/math/float/am9511/readme.md`, section "Am9511 Floating Point Format".
The exponent is an unbiased 7-bit two's complement value from -64 to +63.
The binary point is to the left of mantissa bit 23.
Bit 23 is 1, except that zero is all zeros.
The sign is bit 7 of the high byte.
There is no infinity and no NaN.
The range in that note is about ±(2.7×10⁻²⁰ to 9.2×10¹⁸), and zero.

The next section of that readme is IEEE-754.
z88dk converts IEEE memory values at the APU.
Do not use that conversion for Hi-Tech.
The opcode bytes and the status masks are in `libsrc/newlib/target/am9511/config_am9511_private.inc`.
The AMD datasheet is the document linked from the z88dk readme under `techdocs/amd/am9511a`.

## Two layouts, one cheap conversion

LIBF, `frelop`, `frexp`, `ldexp`, `printf`, and `DEFF` all store the Hi-Tech layout.
Stage 1 does not change those stored bytes.
A change to the stored layout would require new copies of those modules and a new `DEFF`.

The Am9511A stack uses the device layout only.
Push converts Hi-Tech `HLDE` to device bytes.
Pop converts device bytes back to Hi-Tech `HLDE`.
This is the same split that z88dk uses, with a different memory format.
z88dk keeps IEEE-754 in memory.
This library keeps the Hi-Tech layout in memory.

The 24 mantissa bits are already the device mantissa.
The binary point in memory is to the right of that integer.
The binary point on the chip is to the left of bit 23.
Those two views differ by 2^24.
The Hi-Tech bias of 64 absorbs that shift.
The high bytes differ by XOR `0x40` and by nothing else.

Check this on 1.0.

- Hi-Tech 1.0 is exponent field 65 and mantissa `0x800000`.
- `frexp(1.0)` returns 0.5 and stores the power 1.
- Device 1.0 is exponent +1 and the same mantissa.
- `0x01 XOR 0x40` is `0x41`, which is 65.

Further checks for a positive value:

| Device exponent | Device high byte | After XOR `0x40` | Hi-Tech field |
|-----------------|------------------|------------------|---------------|
| 0 | `0x00` | `0x40` | 64 |
| +1 | `0x01` | `0x41` | 65 |
| -1 | `0x7F` | `0x3F` | 63 |
| +63 | `0x3F` | `0x7F` | 127 |
| -64 | `0x40` | `0x00` | 0 |

An add of `0x40` is the wrong operation.
When bit 6 is set, the add carries into the sign bit.
XOR flips bit 6 and leaves the sign alone.

Registers and memory:

- `H` is the sign and the exponent.
- `L` is the most significant mantissa byte.
- `D` is the middle byte.
- `E` is the least significant byte.
- In memory the low address holds `E`.
- The high address holds `H`.
- `iregstore` in `gen/IREGSET.AS` writes `H` at the high address and moves down.

Push to data port `0x42`, after the chip is idle:

1. Write `E`, then `D`, then `L`.
2. Write `H XOR 0x40`.

Pop in that same byte order.
XOR `0x40` on the last byte.
The result is Hi-Tech `HLDE`.

Zero is the exception.
Both layouts use four zero bytes for a clean zero.
`H XOR 0x40` on a zero exponent is `0x40`, which is not a clean zero.
`frelop` compares the high word after it clears the sign.
A dirty zero would not compare equal to a clean zero.
If the mantissa is zero, push four `0x00` bytes.
If the device ZERO bit is set after the command, return `HLDE = 0`.
Do not XOR a device zero.

`fladd` already returns the other operand when one mantissa is zero.
`flmul` already returns a clean zero when either mantissa is zero.
Keep those paths off the chip.
The bit pattern then matches `LIBF`.

A device exponent of -64 converts to Hi-Tech field 0.
`fpnorm` will not emit that field.
`frexp` and `frelop` still accept it, because they subtract 64 from the low 7 bits.
Keep the converted field.
Flush only a zero mantissa.

## Call convention of the kernel

One operand is in `HLDE`.
The other operand is on the Z80 stack under the return address.
The caller pushes the high word, then the low word, then calls.
The routine removes that operand.
The result comes back in `HLDE`.

`flsub` subtracts the stack operand from the `HLDE` operand.
`fldiv` divides the `HLDE` operand by the stack operand.
On the chip, subtract is NOS minus TOS.
Divide is NOS divided by TOS.
Push the `HLDE` operand first.
Push the stack operand second.

`fpnorm` and `negmant` stay as software in the same module.
`LTOF` calls `fpnorm`.
`FTOL` and `FBCD` call `negmant`.
`negmant` is a 24-bit two's complement of `LDE`.
The chip `CHSF` opcode only flips the sign bit.
It is not a substitute for `negmant`.

## Linker rule

LINQ loads a library module when that module defines a symbol that is still undefined.
Libraries are searched in command-line order.
The first module that defines the symbol is kept.
The whole module is loaded.

`FLOAT.OBJ` defines six publics together:

`fpnorm`, `fladd`, `flsub`, `flmul`, `fldiv`, `negmant`

The new module must define all six.
If it defines only `fladd`, LINQ still loads `FLOAT.OBJ` for `flsub`.
The link then fails with a duplicate `fladd`.

These six names have no leading underscore.
A C name such as `_sin` has one underscore.
The kernel module must not export a C name.
`fperr` in `FLOAT.AS` is a private `bss` byte.
It is not a public.
No other `LIBF` module reads it.

`ASFLOAT.AS` stays in `LIBF`.
`asfladd`, `asflsub`, `asflmul`, and `asfldiv` call `iregset`, then `fl*`, then `iregstore`.
`iregset` and `iregstore` are in `LIBC`.
`frelop` is in `gen/FRELOP.AS` and is also in `LIBC`.
It compares Hi-Tech bytes.
It does not call `flsub`.
Do not put a second `frelop` in `LIB9511`.

The driver appends `LIBC.LIB`.
Do not redefine `cret`, `csv`, `ncsv`, `indir`, `iregset`, `iregstore`, `frelop`, or the integer helpers.

## Chip connection

The RC2014 data port is `0x42`.
The status port and the command port are `0x43`.
z88dk records the base in `libsrc/target/rc2014/config_rc2014_private.inc`.

Read status only after bit 7 (`BUSY`, mask `0x80`) is clear.
While `BUSY` is set, the other bits are undefined.
Do not write a command while the chip is busy.

Command bytes below have the service-request bit clear.
Do not set bit 7 of the command.
Leave the chip stack empty on return.
The APU is one shared device.
Stage 1 does not support a nested call from an interrupt.

| Command | Byte |
|---------|------|
| FADD | `0x10` |
| FSUB | `0x11` |
| FMUL | `0x12` |
| FDIV | `0x13` |

Later stages may use these bytes from the same include.
Do not invent an opcode.

| Command | Byte | Command | Byte |
|---------|------|---------|------|
| SQRT | `0x01` | CHSF | `0x15` |
| SIN | `0x02` | PTOF | `0x17` |
| COS | `0x03` | POPF | `0x18` |
| TAN | `0x04` | PUPI | `0x1A` |
| ASIN | `0x05` | FLTS | `0x1D` |
| ACOS | `0x06` | FIXS | `0x1F` |
| ATAN | `0x07` | NOP | `0x00` |
| LOG | `0x08` | LN | `0x09` |
| EXP | `0x0A` | PWR | `0x0B` |

Status after `BUSY` clears:

| Mask | Meaning |
|------|---------|
| `0x40` | SIGN |
| `0x20` | ZERO |
| `0x10` | Divide by zero |
| `0x08` | Square root or log of a negative |
| `0x18` | Domain error (`asin`, `acos`, or `exp`) |
| `0x04` | Underflow |
| `0x02` | Overflow |
| `0x01` | CARRY |

Bits 4 to 1 are one error code.
They are not four independent flags.

## Errors

`fpovrflw` stores 1 in `fperr` and returns zero.
Stage 1 returns zero for overflow, underflow, and divide by zero.
Do not add `errno`, infinity, or NaN.
z88dk maps those codes onto IEEE specials.
Do not copy that map.

`fldiv` in `FLOAT.AS` has no explicit test for a zero divisor.
Measure `1.0/0.0` with `LIBF` alone before you call a difference a fault.
The chip reports divide by zero and leaves the dividend on the stack.
Stage 1 discards that value and returns zero.

`log` and `sqrt` return `0.0` when the argument is not positive.
`pow` returns `0.0` for some negative bases.
Keep that behaviour if a later stage replaces those modules.

## LIBF inventory

`dist/LIBF.LIB` is 31488 bytes.
The librarian directory is record type `0x24` and is 2930 bytes long.
It lists 36 modules.
The order matches `float/MAKELIBF.SUB`.
Object records follow one `0x00` pad byte.
The file tail is `0x1A` padding, not an object record.
`LIBFORIG.LIB` is the older image.
Match `LIBF.LIB`.

In a SYM record the public flag is `0x10`.
`doc/OBJCODE.TXT` writes that flag as octal `020`.
A test for `0x20` misses every public.
An external has low nibble 6.

The kernel column lists `fl*`, `fpnorm`, `negmant`, and `asfl*` only.
Other externals are `LIBC` or another `LIBF` module.

| # | Module | Publics | Kernel use |
|---|--------|---------|------------|
| 01 | `LIBFVER` | `__libfver` | none |
| 02 | `PRINTF` | `_printf` | none |
| 03 | `FPRINTF` | `_fprintf` | none |
| 04 | `SPRINTF` | `_sprintf` | none |
| 05 | `SCANF` | `_scanf` | none |
| 06 | `FSCANF` | `_fscanf` | none |
| 07 | `SSCANF` | `_sscanf` | none |
| 08 | `DOPRNT` | `__doprnt` | none |
| 09 | `DOSCAN` | `__doscan` | through `_atof` |
| 10 | `ATOF` | `_atof` | `fladd` `flmul` `fldiv` |
| 11 | `FNUM` | `__fnum` | through `__fbcd` |
| 12 | `FBCD` | `__fbcd` | `flmul` `negmant` |
| 13 | `TAN` | `_tan` | `fldiv` |
| 14 | `ACOS` | `_acos` | `flsub` |
| 15 | `ASIN` | `_asin` | `flmul` `fldiv` `flsub` |
| 16 | `ATAN2` | `_atan2` | `fldiv`, `asfladd`, `asflsub` |
| 17 | `ATAN` | `_atan` | `flmul` `fldiv` `flsub`, `asflmul` |
| 18 | `COS` | `_cos` | `fladd` `flsub` |
| 19 | `SIN` | `_sin` | `flmul` `fldiv` `flsub`, `asflmul`, `asflsub` |
| 20 | `SINH` | `_sinh` | `flmul` `fldiv` `flsub` |
| 21 | `COSH` | `_cosh` | `fladd` `flmul` `fldiv` |
| 22 | `TANH` | `_tanh` | `fladd` `fldiv` `flsub` |
| 23 | `EXP` | `_exp` `_pow` | `flmul` `fldiv`, `asflmul`, `asflsub`, `ftol`, `lltof` |
| 24 | `LOG` | `_log` `_log10` | `fladd` `flmul` `flsub` |
| 25 | `EVALPOLY` | `_eval_poly` | `fladd` `flmul` |
| 26 | `SQRT` | `_sqrt` | `fladd` `fldiv` |
| 27 | `FREXP` | `_frexp` `_ldexp` | none |
| 28 | `FABS` | `_fabs` | none (`frelop` only) |
| 29 | `CEIL` | `_ceil` | `fladd` |
| 30 | `FLOOR` | `_floor` | `flsub` |
| 31 | `FINC` | `lfinc` `lfdec` | `asfladd` |
| 32 | `ASFLOAT` | `asfladd` `asflsub` `asflmul` `asfldiv` | all four `fl*` |
| 33 | `FRNDINT` | `__frndint` | `ftol` then `altof` |
| 34 | `FTOL` | `ftol` | `negmant` |
| 35 | `LTOF` | `altof` `lltof` `aitof` `litof` `abtof` `lbtof` | `fpnorm` |
| 36 | `FLOAT` | the six kernel names | the module stage 1 replaces |

`pow` is the second function in `float/EXP.C`.
`log10` is the second function in `float/LOG.C`.
`ldexp` is in `float/FREXP.AS` beside `frexp`.
There is no separate object for those names.

`FABS` compares with `frelop` and then negates.
It does not call `fl*`.
Unary minus in the code generator may flip the sign bit.
Do not assume `fabs` uses the APU.

Formatted I/O reaches the kernel through `_atof` and through `__fnum` to `__fbcd`.
Stage 1 does not replace the `printf` or `scanf` modules.

## Names in math.h

`dist/MATH.H` declares:

`fabs`, `floor`, `ceil`, `modf`, `sqrt`, `atof`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `frexp`, `ldexp`, `log`, `log10`, `pow`, `exp`, `sinh`, `cosh`, `tanh`

`modf` is `am9511/MODF.AS`.
The split uses the Hi-Tech exponent field.
`FIXD` is not used. A magnitude at or above 2^31 must still split.

`sin` in `float/SIN.C` divides by `TWO_PI` before the polynomial.
The Am9511A sine, cosine, and tangent take radians.
Pass the argument through unchanged.

## Stages

1. One ZAS module. Publics are the six kernel names. `fladd`, `flsub`, `flmul`, and `fldiv` use the chip. `fpnorm` and `negmant` are the software routines from `FLOAT.AS`. Push and pop use the XOR rule above. Link `LIB9511` before `LIBF`.
2. Optional. Replace `FTOL` and `LTOF` with the chip `FIXS` and `FLTS` opcodes. Each file is its own module. Measure rounding against the current `ftol` path, which adds 1, before the replacement.
3. Optional. Replace chip transcendentals one `LIBF` module at a time. `EXP` must export both `_exp` and `_pow`. `LOG` must export both `_log` and `_log10`. Domain errors return `0.0`.
4. Do not replace `printf`, `scanf`, `FREXP`, or `FABS` in these stages.

## Build on the RC2014

Log onto H: before a compile.
The passes run as `0:NAME` on the logged drive.
Headers need `-IH:`.
Experimental sources and products stay on I:.

Proven float link, software only:

`C -V -IH: I:FILE.C H:LIBF.LIB`

Stage 1 link, from `H>`:

`C -V -IH: I:FILE.C I:LIB9511.LIB H:LIBF.LIB`

`LIB9511` must come first.
If `LIBF` comes first, LINQ keeps the software kernel and never calls the APU.
The driver appends `LIBC.LIB`.
`-LF` was not tested for a library that is not on the logged drive.
Use the drive letter in the name.

A drive letter in `-Oname` does not select the output drive.
The `.COM` lands on the logged drive.
Copy it to I: with PIP.
Erase the H: copy.
Do not erase `H:$EXEC.COM`.
Erase a leftover `.OBJ` and `$$EXEC.$$$`.

Assemble with `ZAS.COM`.
The compiler invokes `ZAS -N -Oname.OBJ`.
Confirm one hand assembly on the machine before a batch.
ZAS writes the object on the logged drive.
Use an object name other than `FLOAT.OBJ`.
Suggested source path in this repo is `AM9511/AM9511.AS`.
Suggested CP/M name is `AM9511.OBJ`.
Build `I:LIB9511.LIB` with `LIBR` key `r`.

ZAS must use official Z80 mnemonics.
Copy the port waits and the opcode bytes.
Do not copy z88dk names, z88dk macros, or the IEEE `pushf` routine.
Do not copy the 8085 sources.

## Tests

Run these on I: after stage 1.

1. The existing float program. `(int)(1.5 * 2.0)` prints `float=3`.
2. Add, subtract, multiply, and divide, with negatives and with zero.
3. A compare and `fabs`, so `frelop` still sees Hi-Tech bytes.
4. `printf` of a float, so the `LIBF` conversion still links.
5. One overflow and one underflow. Both should print 0.
6. Divide by zero. Record it next to the `LIBF` result.

The 1.0 XOR check above is the acceptance check for push and pop.
A wrong add of `0x40` shows up as a sign error on negative exponents.

## Why float text did not print

The float converter was in the loaded program.
The console stayed quiet because `stdout` was not loaded.

`C` links with `LINQ -C100H`.
That writer stops at 128 records.
128 records are 16384 bytes.
The writer returns to the prompt and prints no write error.
BDOS can store a larger file.
`OBJTOHEX -B100H` writes the full image.

The baseline map puts text at 0 for `3F6E` bytes.
Data starts at `3F6E` and ends at `4599`.
`__iob` is at `4181`.
The 128-record file covers `100H` through `40FFH` only.
`printf` runs.
`_flsbuf` finds no `_IOWRT` flag and writes nothing.

A program that fits in 128 records does print.
`I:FPRINT.COM` is 125 records.
It prints `f=1.500000` and `e=1.234500e+04` from `LIBF`.

## Link a program past 128 records

Compile objects only.

`C -V -C -IH: I:FILE.C`

Put the linker line in a file.
End the line with a line feed.
Do not use a carriage return.
The file keeps lower case for `-Ptext=0,data,bss`.
A typed command does not.
The CCP folds typed text to upper case.

```
LINQ <LINK.CMD
OBJTOHEX -B100H L.OBJ NAME.COM
```

The response line for the chip test is:

`-Z -Ptext=0,data,bss -OL.OBJ CRTCPM.OBJ FILE.OBJ I:LIB9511.LIB LIBF.LIB LIBC.LIB`

Copy the COM to I:.
Erase the H: copy.
Do not erase `H:$EXEC.COM`.

Those `C` lines in the build section use `LINQ -C100H`.
Use them only when the COM stays within 128 records.

## Measured on 31 Oct 2026

`I:BASE9511.COM` is 138 records.
It is the `LIBF` baseline with `-DBASE`.
`I:TEST9511.COM` is 143 records.
It links `I:LIB9511.LIB` first.

Both print `start` and `done`.
Add, sub, mul, div, and neg match.
The scaled values are 3750000, 500000, 3000000, 2500000, and -1500000.
`third` is 333333 on both.
`sqrt2` is 1414214 on `LIBF` and 1414213 on the chip.
`ftol` of 1.5 is 2 on `LIBF` and 1 on the chip.
`ftol` of -1.5 is -1 on both.
`modf` on the chip matches the plan values.

The console was left at `H>`.
The kernel commit is on `upstream/master`.

## Work procedure

Follow these steps in order.

1. Commit the current kernel and push it to the local checkout and to `upstream`.
2. Review the existing Am9511A code against the z88dk APU interface.
   Use Z80 timings. `OUT (C),r` and `IN r,(C)` move a register in 12 states.
   `LD A,r` plus `OUT (n),A` takes 15 states.
   An idle busy test is `IN A,(n)`, `RLCA`, and `RET NC`. That is 26 states.
   `BIT 7,A` is slower when the status byte is discarded.
   Wait once, after the command. Data writes do not set `BUSY`.
   Keep one tight poll. A delay loop returns later.
3. Prove each timing change on the RC2014.
   The arithmetic results must match the measured kernel run.
   Time a fixed add and subtract loop before the change and after it.
4. Add the Am9511A float opcodes that `MATH.H` already names.
   Those names are `sqrt`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `log`, `log10`, `exp`, and `pow`.
   The chip sine, cosine, and tangent use radians.
   Link `LIB9511` before `LIBF`.
5. Put chip functions that are not in `MATH.H` in `am9511/AM9511.H`.
   That file includes `MATH.H`.
   Do not add those names to `MATH.H`.
   The added names are `pi`, `log2`, `exp2`, and `exp10`.
6. Assemble on I:, build `LIB9511.LIB`, and run the benchmark.
   Use `OBJTOHEX` when the image passes 128 records.
7. Commit and push the tested library to the local checkout and to `upstream`.
8. Review the new C and assembly for speed, accuracy, and faults.
   Test any repair on the RC2014.
   Commit and push that repair.
9. When the RC2014 console is wrong, type `exit` at the CP/M prompt.
   Start CP/M again from the shell.
10. Record a console fault in `SD-SILENT.md` in the CPM-IDE tree.
    Leave that note until this library work is finished.

## Out of scope

- Do not change `C.COM`, `LIBC.LIB`, `FLOAT.H`, or `MATH.H`.
- Do not change the bytes that `DEFF` and the code generator store.
- Do not copy the z88dk IEEE push path.
- `modf` stays the field split in `MODF.AS`.
- UX Module firmware is a different tree.

## Evidence used for this plan

- `float/MAKELIBF.SUB` and the 36 sources under `float/`.
- `float/FLOAT.AS`, `ASFLOAT.AS`, `FREXP.AS`, `FTOL.AS`, `LTOF.AS`, `FBCD.AS`, `FINC.AS`, `SIN.C`, `EXP.C`, `LOG.C`, `SQRT.C`, `FABS.C`.
- `gen/FRELOP.AS` and `gen/IREGSET.AS`.
- `dist/LIBF.LIB` symbol walk on 31 Oct 2026. 36 modules. Public flag `0x10`.
- `dist/MATH.H` and the `#if z80` block of `dist/FLOAT.H`.
- `doc/HTCZ80.TXT` sections for `DEFF` and for `frexp`.
- `doc/OBJCODE.TXT` for the octal public flag.
- z88dk `libsrc/math/float/am9511/readme.md`.
- z88dk `config_am9511_private.inc` opcodes and `config_rc2014_private.inc` port `0x42`.
- A live software compile already printed `float=3` from `I:FLOAT.C` linked with `H:LIBF.LIB`.

## Review repairs

`ftol` returns 0 when a magnitude does not fit in a signed long.
`floor` and `ceil` in LIBF call `__frndint`, and that routine calls `ftol`.
A value at or above 2^31 would then become 0.
`FLOOR.AS` exports `_floor` and `_ceil`.
The split uses the exponent field, as `modf` does.
`FIXD` is not used.

`exp2` and `exp10` multiply by a constant and then call `EXP`.
A product that overflows is a zero from `popf`.
`EXP` of that zero would return 1.
The routine returns 0 when the input magnitude is at or above 1 and the product is 0.
A smaller input that underflows still returns 1, because `exp(0)` is 1.

`pow` of a base whose magnitude is 1 does not call `PWR`.
A positive 1 returns 1.
A negative 1 uses the integer test and returns 1, -1, or 0.
