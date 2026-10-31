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

	global	ftol, pushf, popl

	psect	text

APUCMD	equ	43h
FIXD	equ	1eh

ftol:
	ld	a,l
	or	d
	or	e
	jr	z,ftzero
	call	pushf
	ld	a,FIXD
	out	(APUCMD),a
	jp	popl
ftzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret
