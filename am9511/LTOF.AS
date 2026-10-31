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
	global	fladd

	psect	text

APUDAT	equ	42h
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
	jp	fladd

altof:
lpos:
	call	lpush
	ld	a,FLTD
	call	lcmd
	jp	lpopf

lwait:
	push	bc
	ld	bc,0
lw1:
	in	a,(APUCMD)
	bit	7,a
	jr	z,lwdn
	dec	bc
	ld	a,b
	or	c
	jr	nz,lw1
	ld	a,0ffh
lwdn:
	pop	bc
	ret

lcmd:
	push	af
	call	lwait
	pop	af
	out	(APUCMD),a
	ret

lpush:
	call	lwait
	ld	a,e
	out	(APUDAT),a
	ld	a,d
	out	(APUDAT),a
	ld	a,l
	out	(APUDAT),a
	ld	a,h
	out	(APUDAT),a
	ret

lpopf:
	call	lwait
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
	jr	nz,lzero
	ld	a,c
	and	20h
	jr	nz,lzero
	ld	a,h
	xor	40h
	ld	h,a
	ld	a,l
	or	d
	or	e
	ret	nz
lzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret
