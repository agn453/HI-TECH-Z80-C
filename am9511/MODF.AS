;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	double modf(double x, double *ip)
;	Store the integer part of x through ip. Return the fraction.
;	Both parts carry the sign of x. A zero fraction is a clean zero.
;	The split uses the Hi-Tech field, so it covers the whole format.
;	The fraction is x minus the integer part through flsub.

	global	_modf, flsub

	psect	text

_modf:
	push	ix
	ld	ix,0
	add	ix,sp
	ld	e,(ix+4)
	ld	d,(ix+5)
	ld	l,(ix+6)
	ld	h,(ix+7)
	ld	a,h
	and	7fh
	sub	64
	jr	c,mfrac
	jr	z,mfrac
	cp	24
	jr	nc,mwhole
	push	hl
	push	de
	ld	b,a
	ld	a,24
	sub	b
	ld	c,a
	ld	b,c
mshr:
	srl	l
	rr	d
	rr	e
	djnz	mshr
	ld	b,c
mshl:
	sla	e
	rl	d
	rl	l
	djnz	mshl
	call	mstor
	exx
	pop	de
	pop	hl
	exx
	push	hl
	push	de
	exx
	call	flsub
	pop	ix
	ret

mfrac:
	push	hl
	push	de
	ld	hl,0
	ld	e,l
	ld	d,h
	call	mstor
	pop	de
	pop	hl
	pop	ix
	ret

mwhole:
	call	mstor
	ld	hl,0
	ld	e,l
	ld	d,h
	pop	ix
	ret

mstor:
	ld	c,(ix+8)
	ld	b,(ix+9)
	ld	a,e
	ld	(bc),a
	inc	bc
	ld	a,d
	ld	(bc),a
	inc	bc
	ld	a,l
	ld	(bc),a
	inc	bc
	ld	a,h
	ld	(bc),a
	ret
