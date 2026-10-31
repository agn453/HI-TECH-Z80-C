;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Integer to Hi-Tech float through FLTD (1Ch).
;	FLTS (1Dh) is the 16-bit opcode. Every entry widens to 32 bits
;	and uses FLTD.
;	altof is a signed long. lltof is an unsigned long.
;	A unsigned value above 7FFFFFFFh is converted in two parts.

	global	altof, lltof, aitof, litof, abtof, lbtof
	global	fladd, pushl, popf

	psect	text

APUCMD	equ	43h
FLTD	equ	1ch

lbtof:
	ld	e,a
	ld	d,0
	ld	hl,0
	jp	lltof

litof:
	ex	de,hl
	ld	hl,0
	jp	lltof

abtof:
	ld	l,a
	rla
	sbc	a,a
	ld	h,a

aitof:
	ld	a,h
	ex	de,hl
	rla
	sbc	a,a
	ld	h,a
	ld	l,a
	jp	altof

lltof:
	bit	7,h
	jr	z,lpos
	res	7,h
	call	lpos
	ld	bc,6080h
	push	bc
	ld	bc,0
	push	bc
	call	fladd
	ret

altof:
lpos:
	call	pushl
	ld	a,FLTD
	out	(APUCMD),a
	jp	popf
