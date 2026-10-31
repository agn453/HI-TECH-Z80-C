;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	ftol - Hi-Tech float in HLDE to a signed long in HLDE.
;	FIXD (1Eh) is the 32-bit chip conversion. FIXS (1Fh) is 16-bit.
;	The chip truncates toward zero. A result that does not fit
;	in a signed long returns 0.

	global	ftol

	psect	text

APUDAT	equ	42h
APUCMD	equ	43h
FIXD	equ	1eh

ftol:
	ld	a,l
	or	d
	or	e
	jr	z,ftzero
	call	fpush
	ld	a,FIXD
	call	fcmd
	jp	fpopl
ftzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

fwait:
	push	bc
	ld	bc,0
fw1:
	in	a,(APUCMD)
	bit	7,a
	jr	z,fwdn
	dec	bc
	ld	a,b
	or	c
	jr	nz,fw1
	ld	a,0ffh
fwdn:
	pop	bc
	ret

fcmd:
	push	af
	call	fwait
	pop	af
	out	(APUCMD),a
	ret

fpush:
	call	fwait
	ld	a,l
	or	d
	or	e
	jr	z,fpz
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
fpz:
	xor	a
	out	(APUDAT),a
	out	(APUDAT),a
	out	(APUDAT),a
	out	(APUDAT),a
	ret

;	Pop a 32-bit integer. High byte first. No exponent XOR.

fpopl:
	call	fwait
	in	a,(APUDAT)
	ld	h,a
	in	a,(APUDAT)
	ld	l,a
	in	a,(APUDAT)
	ld	d,a
	in	a,(APUDAT)
	ld	e,a
	in	a,(APUCMD)
	and	1eh
	ret	z
	ld	hl,0
	ld	e,l
	ld	d,h
	ret
