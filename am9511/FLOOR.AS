;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	floor and ceil for the whole Hi-Tech range.
;	ftol returns 0 when the magnitude does not fit in a signed long.
;	LIBF floor and ceil call that path, so a large value would become 0.
;	The split uses the exponent field. The chip is not required.
;	A value with field-64 at or above 24 is already an integer.

	global	_floor, _ceil, fladd, flsub

	psect	text

_floor:
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	a,l
	or	d
	or	e
	ret	z
	call	fexp
	jr	c,flsmall
	jr	z,flsmall
	cp	24
	ret	nc
	call	ftrunc
	ret	z
	bit	7,h
	ret	z
	jr	fminus1

flsmall:
	bit	7,h
	jr	nz,fm1
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

_ceil:
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	a,l
	or	d
	or	e
	ret	z
	call	fexp
	jr	c,cesmall
	jr	z,cesmall
	cp	24
	ret	nc
	call	ftrunc
	ret	z
	bit	7,h
	jr	z,fplus1
	ret

cesmall:
	bit	7,h
	jr	nz,cezero
	ld	hl,4180h
	ld	de,0
	ret
cezero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

;	A holds field-64. Carry means the field was below 64.

fexp:
	ld	a,h
	and	7fh
	sub	64
	ret

;	Clear the fraction bits. Z is set when no bit was shifted out.
;	H is unchanged. A holds the unbiased exponent on entry.

ftrunc:
	ld	c,a
	ld	a,24
	sub	c
	ld	b,a
	ld	c,b
	xor	a
ftr:
	srl	l
	rr	d
	rr	e
	jr	nc,fts
	ld	a,1
fts:
	djnz	ftr
	ld	b,c
ftl:
	sla	e
	rl	d
	rl	l
	djnz	ftl
	or	a
	ret

fminus1:
	ld	bc,4180h
	push	bc
	ld	bc,0
	push	bc
	call	flsub
	ret

fplus1:
	ld	bc,4180h
	push	bc
	ld	bc,0
	push	bc
	call	fladd
	ret

fm1:
	ld	hl,0c180h
	ld	de,0
	ret
