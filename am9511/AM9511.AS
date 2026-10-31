;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Am9511A kernel for Hi-Tech Z80 float.
;
;	Memory values stay in the Hi-Tech layout. The chip stack uses
;	the Am9511A layout. The mantissa bytes match. The sign and
;	exponent byte differs by XOR 40h.
;	Data port 42h. Status and command port 43h.
;	Push sends the low byte first. Pop returns the high byte first.
;	A zero mantissa is four 00h bytes on the chip.
;	Overflow, underflow, and divide by zero return a clean zero.

	global	fpnorm, fladd, flsub, flmul, fldiv, negmant

	psect	text

APUDAT	equ	42h
APUCMD	equ	43h
FADD	equ	10h
FSUB	equ	11h
FMUL	equ	12h
FDIV	equ	13h

;	fpnorm - float in HLDE. H holds the sign and the exponent.

fpnorm:
	ld	a,l
	or	d
	or	e
	jp	z,fpzero
	push	hl
	pop	bc
	ld	c,b
	res	7,c
fpn2:
	bit	7,l
	jr	nz,fpn3
	dec	c
	jp	m,fpovrflw
	ex	de,hl
	add	hl,hl
	ex	de,hl
	adc	hl,hl
	jr	fpn2
fpn3:
	bit	7,b
	jr	z,fpn4
	set	7,c
fpn4:
	ld	h,c
	ret

fpovrflw:
	ld	a,1
	ld	(fperr),a
fpzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

;	Negate the 24-bit mantissa in LDE. H is unchanged.

negmant:
	push	hl
	ld	hl,0
	ld	a,l
	or	a
	sbc	hl,de
	ex	de,hl
	pop	hl
	sbc	a,l
	ld	l,a
	ret

;	HLDE minus the stack operand. A-B == A+(-B).

flsub:
	pop	bc
	exx
	pop	de
	pop	hl
	ld	a,h
	or	a
	jr	z,flsub1
	xor	80h
	ld	h,a
flsub1:
	push	hl
	push	de
	exx
	push	bc
	jp	fladd

;	Add HLDE to the stack operand. Remove the operand.
;	A zero mantissa returns the other operand unchanged.

fladd:
	ld	a,l
	or	d
	or	e
	exx
	pop	bc
	pop	de
	pop	hl
	push	bc
	ret	z
	ld	a,l
	or	d
	or	e
	exx
	ret	z
	call	pushf
	exx
	call	pushf
	exx
	ld	a,FADD
	call	apcmd
	jp	popf

;	Multiply HLDE by the stack operand. A zero mantissa returns 0.

flmul:
	ld	a,l
	or	d
	or	e
	exx
	pop	bc
	pop	de
	pop	hl
	push	bc
	jr	z,flzero
	ld	a,l
	or	d
	or	e
	exx
	jr	z,flzero
	call	pushf
	exx
	call	pushf
	exx
	ld	a,FMUL
	call	apcmd
	jp	popf

;	Divide HLDE by the stack operand.
;	A zero dividend or a zero divisor returns 0.

fldiv:
	ld	a,l
	or	d
	or	e
	exx
	pop	bc
	pop	de
	pop	hl
	push	bc
	jr	z,flzero
	ld	a,l
	or	d
	or	e
	exx
	jr	z,flzero
	call	pushf
	exx
	call	pushf
	exx
	ld	a,FDIV
	call	apcmd
	jp	popf

flzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

;	Wait until BUSY is clear. Return the status in A.
;	A missing chip returns FFh so the caller can give up.

apwait:
	push	bc
	ld	bc,0
apw1:
	in	a,(APUCMD)
	bit	7,a
	jr	z,apwdn
	dec	bc
	ld	a,b
	or	c
	jr	nz,apw1
	ld	a,0ffh
apwdn:
	pop	bc
	ret

;	Start the command in A. Bit 7 stays clear.

apcmd:
	push	af
	call	apwait
	pop	af
	out	(APUCMD),a
	ret

;	Push HLDE. Low byte first. XOR 40h on the exponent byte.

pushf:
	call	apwait
	ld	a,l
	or	d
	or	e
	jr	z,push0
	ld	a,e
	out	(APUDAT),a
	ld	a,d
	out	(APUDAT),a
	ld	a,l
	out	(APUDAT),a
	ld	a,h
	xor	40h
	out	(APUDAT),a
	ret
push0:
	xor	a
	out	(APUDAT),a
	out	(APUDAT),a
	out	(APUDAT),a
	out	(APUDAT),a
	ret

;	Pop a float into HLDE. High byte first.
;	ZERO or an error code returns a clean zero.

popf:
	call	apwait
	in	a,(APUDAT)
	ld	h,a
	in	a,(APUDAT)
	ld	l,a
	in	a,(APUDAT)
	ld	d,a
	in	a,(APUDAT)
	ld	e,a
	in	a,(APUCMD)
	ld	c,a
	and	1eh
	jp	nz,fpovrflw
	ld	a,c
	and	20h
	jp	nz,fpzero
	ld	a,h
	xor	40h
	ld	h,a
	ld	a,l
	or	d
	or	e
	ret	nz
	jp	fpzero

	psect	bss
fperr:
	defs	1
	psect	text
