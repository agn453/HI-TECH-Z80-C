;-----------------------------------------------------------------------------
;
; All BDOS calls on all versions of CP/M 80 return with B = H and A = L.
; Furthermore, when an 8-bit result is all that is returned then B = H = 0.
;
; This is a documented feature of CP/M but it seems to have been overlooked
; by most programmers.  Compiler vendors in particular seem to have felt
; compelled to provide two versions of the bdos() function, one for 8-bit
; results and one for 16-bit results.  There is simply no need for that.
;
; This is a replacement for the bdos() and bdoshl() functions in the Hi-Tech
; C runtime library.
;
; How to patch your library:
;
;	zas bdos.as
;	libr d libc.lib bdoshl.obj
;	libr r libc.lib bdos.obj
;
; Jon Saxton
; September 2012
;
;-----------------------------------------------------------------------------
;
; There is one caveat attached to all this.  The Hi-Tech C bdos() function
; did a sign extension of the 8-bit result to convert it to a 16-bit result.
; That is no longer done.  It is only going to be a problem if your code
; checks 8-bit return codes for negative numbers, e.g. -1 instead of 0xFF.
; The function prototype is:
;
;	short bdos(short func, ...);
;
; instead of:
;
;	char bdos(short func, ...);
;	short bdoshl(short func, ...);
;
; When running under CP/M 3 there are several functions which can return
; extended error codes.  For those functions, the error signal is A=L=0xFF
; and the extended error is in B and H.  For consistency across all versions
; of CP/M it is correct to test the low-order byte of the return value where
; appropriate; for example:
;
;	short	rc;
;	...
;	rc = bdos(fn, arg);
;	if ((rc & 0xFF) == 0xFF)
;	    [deal with error];
;	else
;	    [normal processing];
;
; Note that finding 0xFF in the low-order 8 bits of the return value does not
; always indicate an error.  For example, functions such as 25 (return login
; vector) and 31 (get DPB address) could legitimately return such a value.
; When calling the BDOS one always has to be aware of the context and the
; possible return value.  The prior implementation with separate calls for
; for 8-bit and 16-bit results was already an expression of context awareness.
;
; Also be aware that an error is not always flagged by 0xFF in the low-order
; byte of the return value.  For some functions any non-zero return value is
; an error.
;
;-----------------------------------------------------------------------------
;
; 10 Apr 2014
;
; This module has been updated to incorporate John Elliott's code to set
; _errno on a failed BDOS call.
;
; This is only possible on CP/M Plus and only meaningful for the subset of
; BDOS functions which return an extended error code.  These functions can
; now set errno.
;
; John's version of this routine is bdose() and is part of the PIPEMGR suite
; found on his web site:
;
;	http://www.seasip.demon.co.uk/Cpm/software/Pipemgr/index.html
;
; There is no need to call bdose(); bdos() does everything.
;
;-----------------------------------------------------------------------------
;
; The password handler mechanism documented in John's LIBCNEW.DOC has been
; disabled for the time being because it seems to be ineffective and I have
; not yet discovered why.  It is possible that the problem lies with CP/M 3
; rather than John's code.  Opening a password-protected file generates a
; password error but entering a null password is sufficient to bypass the
; protection!
;
; Passwords are probably useless anyway.  They are also easily bypassed.
;
; Jon Saxton [20 Apr 2014]
;
;-----------------------------------------------------------------------------

	global	csv,cret

false	equ	0
true	equ	.not. false

PWDREC	equ	false		; Password recovery mechanism (off)

entry	equ	5		; CP/M entry point

arg	equ	8		;argument to call
func	equ	6		;desired function

	global	_bdos, _errno

	cond	PWDREC
	global	__passwd, __dpass
	endc

	psect	data

	cond	PWDREC
__passwd:
	defw	__dpass
	endc

;-----------------------------------------------------------------------------
; The following table is a bit map.  Each bit represents a BDOS function
; which, under CP/M Plus, can return an extended error code.  The functions
; identified in the bit map are also the ones which can set errno.
;
; The extended errors are consistent over all the relevant functions:
;
;	Error		Meaning
;
;	  1		Disk I/O error
;	  2		Read-only disk
;	  3		Read-only file
;	  4		Invalid drive
;	  5
;	  6
;	  7		Password error
;	  8		File exists
;	  9		? in file name
;
; These are the functions flagged in the bitmap.
;
;	BDOS function			Possible errors 
;
;	14 Select disk			1, 4
;	15 Open file			1, 4, 7, 9
;	16 Close file			1, 2, 4
;	17 Search for first		1, 4
;	18 Search for next		1, 4
;	19 Delete file			1, 2, 3, 4, 7
;	20 Sequential read		1, 4
;	21 Sequential write		1, 2, 3, 4
;	22 Create file			1, 2, 4, 8, 9
;	23 Rename file			1, 2, 3, 4, 7, 8, 9
;	30 Set file attributes		1, 2, 4, 7, 9
;	33 Random read			1, 4
;	34 Random write			1, 2, 3, 4
;	35 Compute file size		1, 4
;	40 Random write with zero fill	1, 2, 3, 4
;	46 Get disk free space		1, 4
;	48 Flush buffers		1, 2, 4
;	59 Load overlay			1, 4
;	60 Call RSX			depends on RSX
;	98 Free blocks			4
;	99 Truncate file		1, 2, 3, 4, 7, 9
;      100 Set directory label		1, 2, 4, 7
;      101 Return directory label data	1, 4
;      102 File datestamps/pwd mode	1, 4, 9
;      103 Write file XFCB		1, 2, 4, 7, 9
;-----------------------------------------------------------------------------
 
funcmap:
	defb	00000000B	;   0
	defb	11000000B	;   8		14,15
	defb	11111111B	;  16		16-23
	defb	01000000B	;  24		30
	defb	00001110B	;  32		33-35
	defb	01000001B	;  40		40,46
	defb	00000001B	;  48		48
	defb	00011000B	;  56		59,60
	defb	00000000B	;  64
	defb	00000000B	;  72
	defb	00000000B	;  80
	defb	00000000B	;  88
	defb	11111100B	;  96		98-103
	defb	00000000B	; 104
	defb	00000000B	; 112
	defb	00000000B	; 120
	defb	00000000B	; 128
	defb	00000000B	; 136
	defb	00000000B	; 144
	defb	00000000B	; 152

	psect	text

	cond	PWDREC
__dpass:
	ld	hl,0
	ret
	endc

_bdos:
	call	csv		; Establish pointer to parameters
retry:
	ld	e,(ix+arg)
	ld	d,(ix+arg+1)
	ld	c,(ix+func)
	push	iy		; Save IY for caller
	push	ix		; Save stack pointer over BDOS call
	call	entry		; Call BDOS
	pop	ix		; Recover stack pointer
	ld	a,(ix+func)	; Get BDOS function number
	cp	160		; Check range
	jr	nc,2f		; Skip extended processing if out of range
	push	hl		; Save BDOS return code
	ld	c,12		; Get CP/M version
	call	entry
	pop	hl		; Restore BDOS result before checking version
	cp	30h		; Need at least 3.x
	jr	c,2f		; No extended error processing if not CP/M+
	ld	iy,instruction	; Prepare to modify code
	ld	a,(ix+func)	; Get function number again
	push	af		; Save it for a moment
	and	7		; Isolate bit number
	add	a,a		; Shift left 3 positions
	add	a,a		;  /
	add	a,a		; /
	or	46h		; Form BIT sub-instruction
	ld	(iy+3),a	; Store it
	pop	af		; Recover function number
	srl	a		; Shift right 3 positions to form byte index
	srl	a		;  /
	srl	a		; /
	ld	(iy+2),a	; Store offset
	ld	iy,funcmap	; Point at function bitmap
instruction:
	bit	0,(iy+0)	; This gets modified by foregoing code
	jr	z,2f		; Not a function returning an extended error
	ld	a,l		; Check for error
	inc	a		; Only 0xFF indicates an extended error
	ld	a,-16		; Pre-condition for a zero
	jr	nz,0f		; Skip if extended error is not possible
	ld	a,h		; Get extended error code

;-----------------------------------------------------------------------------
; The following code exists to support a user-supplied password routine.

	cond	PWDREC
	cp	7		; Password error?
	jr	nz,0f		; If not then just set _errno and exit
	ld	bc,pwret
	push	bc		; Set return address
	ld	hl,(__passwd)	; Possible user-supplied password handler
	jp	(hl)		; Execute default or user-supplied handler
pwret:
	ld	a,h		; Check to see if a password came back
	or	l
	ld	a,7		; Prepare a password error code
	jr	z,0f		; Exit with errno=23 if no password
	ex	de,hl		; DE = password address
	ld	c,26		; Set DMA
	push	ix
	call	entry		; Point DMA at the password
	pop	ix
	pop	iy
	jp	retry		; DE is still on the stack!
	endc
;-----------------------------------------------------------------------------

0:
	add	a,16
	ld	(_errno),a
2:
	pop	iy
	jp	cret
