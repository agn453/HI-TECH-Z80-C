;------------------------------------------------------------------------------
;
; Modified bios() function for Hi-Tech C.
;
; The original code simply acessed the jump table at the start of the CP/M
; BIOS.  That is unsupported (and potentially dangerous) under CP/M 3.x with
; banked memory.
;
; The sanctioned method on CP/M 3.x is to use BDOS function 50.  Since that
; won't work on CP/M 2.2 this version of bios() tests the operating system
; version and acts according to the answer it receives.
;
; But that is not all.  CP/M 2.2 BIOS calls take 0, 1 or 2 inputs.  Single
; inputs go in BC or C, double inputs go in BC and DE.  CP/M 3.x has BDOS
; functions which take inputs in other registers.  The bios() function is
; smart enough to figure out where the arguments go.
;
; Since some functions return 16-bit results, mask the return when expecting
; an 8-bit result.
;
; Examples:
;	rc = bios(2) & 0xFF;		/* Get console input status */
;	bios(4,'K');			/* Write a K to the console */
;	rc = bios(16, sector, trntbl);	/* Get translated sector number */
;	bios(len, src, dst);		/* Copy len bytes from src to dst */s
;
; There was a problem with bios() calls under CP/M 2.2.  The original code
; always returned the A register contents.  In other words it NEVER returned
; a 16-bit result.  That has been fixed.k
;
; Jon Saxton
; (ex-)sysop of the long-defunct Tesseract RCPM+
; 2010-01-30
;
;------------------------------------------------------------------------------

	psect	text

	global	_bios, csv, cret
	global	exit22

cpm	equ	 5			;bdos entry point
version	equ	12
arg	equ	 6			;offset of 1st arg on stack

SELDSK	equ	 9
SECTRN	equ	16
DEVTBL	equ	20
DRVTBL	equ	22
MOVE	equ	25
USERF	equ	30

_bios:
	call	csv
	ld	c,version
	call	cpm
	cp	30h
	jr	nc,30f

;==============================================================================
;			   BIOS call for CP/M 2.2
;==============================================================================

	ld	hl,21f			;set up return address
	push	hl
	ld	l,(ix+arg+0)		;get bios index
	dec	l			;adjust
	ld	e,l
	ld	h,0
	ld	d,0
	add	hl,hl			;triple it
	add	hl,de
	ld	de,(1)			;get warm boot address
	add	hl,de			;now have transfer address
	push	hl			;put on stack
	ld	c,(ix+arg+2)		;get 1st arg
	ld	b,(ix+arg+3)
	ld	e,(ix+arg+4)		;get 2nd arg
	ld	d,(ix+arg+5)
	ret				;do bios call
21:
	; Check for 16-bit or 8-bit result.  For CP/M 2.2 the only functions
	; which return 16-bit results are SELDSK (9) and SECTRAN (16).
	; This code is shared with CP/M 3 and that operating system has a few
	; more calls which may return a 16-bit result.
exit22:
	ld	c,a			; Save possible 8-bit return code
	ld	a,(ix+arg+0)		; Get function
	cp	SELDSK			; Test for 16-bit return
	jr	z,23f
	cp	SECTRN
	jr	z,23f
	cp	DEVTBL
	jr	z,23f
	cp	DRVTBL
	jr	z,23f
	cp	MOVE
	jr	z,23f
	cp	USERF
	jr	nc,23f
	ld	l,c			;return value in a (copied to C)
	ld	h,0
23:
	jp	cret

;==============================================================================
;			    BIOS call for CP/M 3.x
;==============================================================================

30:
	push	iy
	ld	iy,biospb		; Point at BIOS parameter block
	ld	e,(ix+arg+0)		; Get function number
	ld	(iy+0),e		; Store in BPB
	ld	hl,rutable		; Point at table
	ld	d,0
	srl	e			; Halve the index, low order bit to CF
	push	af			; Save carry
	add	hl,de			; Point at function bits
	pop	af			; Restore carry flag
	ld	a,(hl)			; Get register usage flags
	jr	nc,31f			; Skip if index was even
	rrca				; Index was odd, move high nybble down
	rrca
	rrca
	rrca
31:
	push	ix			; Stack frame to HL
	pop	hl
	ld	de,arg+2		; Point at first argument
	add	hl,de
	ld	e,(hl)			; Load first argument
	inc	hl
	ld	d,(hl)
	inc	hl
	rra				; Low bit of register flags to carry
	jr	nc,34f			; Skip if A register not used
	ld	(iy+1),e		; Store argument in BPB.A
	ld	e,(hl)			; Get next argument
	inc	hl
	ld	d,(hl)
	inc	hl
34:
	ld	b,3			; Shift counter
35:
	inc	iy			; Step the BPB to the next register
	inc	iy			;  slot
	rra				; Register usage bit to carry
	jr	nc,36f			; Skip if unused
	ld	(iy+0),e		; Store argument in BPB.XX
	ld	(iy+1),d		;  where XX is BC, DE or HL
	ld	e,(hl)			; Get next argument
	inc	hl
	ld	d,(hl)
	inc	hl
36:
	djnz	35b

	pop	iy			; Restore caller's IY
	ld	de,biospb		; Recover BPB address
	ld	c,50			; BIOS call via BDOS
	call	cpm
	jr	21b			; Exit with 8- or 16-bit result

;==============================================================================

	psect	data
biospb:
	defs	8			; BIOS parameter block

;------------------------------------------------------------------------------
; The following table defines input register usage for BIOS functions.
;
;	Bit 0 is set if A is used
;	Bit 1 is set if BC is used
;	Bit 2 is set if DE is used
;	Bit 3 is set if HL is used
;
; Each entry in the table represents two functions.  The lower nybble holds
; the register usage bits for an even-numbered function and the upper nybble
; holds the bits for the next (odd-numbered) function.
;------------------------------------------------------------------------------

rutable:
	defb	00h			;  0 - Cold boot
					;  1 - Warm boot
	defb	00h			;  2 - Console input status
					;  3 - Console input
	defb	22h			;  4 - Console output
					;  5 - List device output
	defb	02h			;  6 - Auxiliary device output
					;  7 - Auxiliary device input
	defb	60h			;  8 - Home disk
					;  9 - Select disk
	defb	22h			; 10 - Set track
					; 11 - Set sector
	defb	02h			; 12 - Set DMA
					; 13 - Read
	defb	02h			; 14 - Write
					; 15 - List device status
	defb	06h			; 16 - Sector translate
					; 17 - Console output status
	defb	00h			; 18 - Auxiliary input status
					; 19 - Auxiliary output status
	defb	20h			; 20 - Get device table address
					; 21 - Initialise device
	defb	20h			; 22 - Drive table
					; 23 - Multi sector I/O
	defb	0E0h			; 24 - Flush buffers
					; 25 - Move
	defb	12h			; 26 - Time
					; 27 - Select memory bank
	defb	21h			; 28 - Set bank for I/O
					; 29 - XMove
	defb	0FFh			; 30 - UserF
					; 31 - Reserved 1
	defb	0Fh			; 32 - Reserved 2

;==============================================================================
;	BIOS call summary for C programs
;==============================================================================
;
;	bios(0);			Cold boot
;	bios(1);			Warm boot
;	rc = bios(2);			Console input status
;	rc = bios(3);			Console input
;	bios(4, chr);			Console output
;	bios(5,	chr);			List device output
;	bios(6, chr);			Auxiliary device output (PUN)
;	rc = bios(7);			Auxiliary device input (RDR)
;	bios(8);			Home disk
;	rc = bios(9, drv, init);	Select disk
;	bios(10, track);		Set track
;	bios(11, sector);		Set sector
;	bios(12, pDMA);			Set DMA
;	rc = bios(13);			Read
;	rc = bios(14, deblock);		Write
;	rc = bios(15);			List device status
;	rc = bios(16, sector, txtbl);	Sector translate
;
;-------------------------- CP/M 2.2 - 3.1 boundary ---------------------------
;
;	rc = bios(17);			Console output status
;	rc = bios(18);			Auxiliary input status
;	rc = bios(19);			Auxiliary output status
;	rc = bios(20);			Get device table address
;	rc = bios(21, devno);		Initialise device
;	rc = bios(22);			Drive table
;	bios(23, sectors);		Multi sector I/O
;	rc = bios(24);			Flush buffers
;	bios(25, len, src, dest);	Move
;	bios(26, getorset);		Time
;	bios(27, bank);			Select memory bank
;	bios(28, iobank);		Set bank for I/O
;	bios(29, srcBank+destBank*256);	XMove
;	rc = bios(30, a, bc, de, hl);	UserF (implementor defined)
;	rc = bios(31, a, bc, de, hl);	Reserved
;	rc = bios(32, a, bc, de, hl);	Reserved
;==============================================================================
