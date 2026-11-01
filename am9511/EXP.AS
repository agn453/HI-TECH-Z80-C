;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	exp and pow.
;	exp is the chip EXP opcode.
;	pow matches the LIBF special cases.
;	x == 0 returns 0, including 0^0.
;	y == 0 returns 1.
;	A negative base with a non-integer exponent returns 0.
;	An odd integer exponent restores the sign.
;	An integer exponent below 2^24 uses FMUL.
;	The product is exact when it fits in the mantissa.
;	Any other exponent uses the chip PWR opcode.
;	The chip PWR opcode sees a positive base only.

	global	_exp, _pow, cunary, pushf, popf, flmul, fldiv

	psect	text

APUCMD	equ	43h
OP_EXP	equ	0ah
OP_PWR	equ	0bh

_exp:
	ld	a,OP_EXP
	jp	cunary

;	Stack on entry is return, x, y. Both arguments stay there.

_pow:
	pop	bc
	pop	de
	pop	hl
	exx
	pop	de
	pop	hl
	push	hl
	push	de
	exx
	push	hl
	push	de
	push	bc
	ld	a,l
	or	d
	or	e
	jr	z,pzero
	exx
	ld	a,l
	or	d
	or	e
	exx
	jr	z,pone
	ld	a,l
	cp	80h
	jr	nz,psign
	ld	a,d
	or	e
	jr	nz,psign
	ld	a,h
	and	7fh
	cp	41h
	jr	nz,psign
	bit	7,h
	jr	z,pone
	push	hl
	push	de
	exx
	push	hl
	push	de
	call	yodd
	pop	de
	pop	hl
	exx
	pop	de
	pop	hl
	jr	c,pzero
	jr	z,pone
	ld	hl,0c180h
	ld	de,0
	ret
psign:
	bit	7,h
	jr	z,ppwr
	push	hl
	push	de
	exx
	push	hl
	push	de
	call	yodd
	pop	de
	pop	hl
	exx
	pop	de
	pop	hl
	jr	c,pzero
	res	7,h
	jr	z,ppwr
	call	ppwr
	ld	a,l
	or	d
	or	e
	ret	z
	ld	a,h
	xor	80h
	ld	h,a
	ret

pzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

pone:
	ld	hl,4180h
	ld	de,0
	ret

;	x is in the main bank. y is in the alternate bank.
;	An integer y below 2^24 uses FMUL.
;	Any other y uses chip PWR. Push x, then y.

ppwr:
	exx
	push	hl
	push	de
	call	yival
	jr	c,ppchip
	pop	bc
	pop	bc
	jp	pint

ppchip:
	pop	de
	pop	hl
	exx
	call	pushf
	exx
	call	pushf
	exx
	ld	a,OP_PWR
	out	(APUCMD),a
	jp	popf

;	Main bank holds the sign in H and the absolute exponent in LDE.
;	Alternate bank holds x. A set sign returns the reciprocal.
;	IX+0 is the saved IX. x is at IX+2. The exponent is at IX+6.

pint:
	push	de
	ld	e,l
	ld	d,h
	push	de
	exx
	push	hl
	push	de
	push	ix
	ld	ix,0
	add	ix,sp
	ld	hl,4180h
	ld	de,0
pbit:
	ld	a,(ix+6)
	or	(ix+8)
	or	(ix+9)
	jr	z,pinv
	srl	(ix+6)
	rr	(ix+9)
	rr	(ix+8)
	jr	nc,psq
	ld	b,(ix+5)
	ld	c,(ix+4)
	push	bc
	ld	b,(ix+3)
	ld	c,(ix+2)
	push	bc
	call	flmul
psq:
	ld	a,(ix+6)
	or	(ix+8)
	or	(ix+9)
	jr	z,pinv
	push	hl
	push	de
	ld	l,(ix+4)
	ld	h,(ix+5)
	ld	e,(ix+2)
	ld	d,(ix+3)
	ld	b,h
	ld	c,l
	push	bc
	ld	b,d
	ld	c,e
	push	bc
	call	flmul
	ld	(ix+2),e
	ld	(ix+3),d
	ld	(ix+4),l
	ld	(ix+5),h
	pop	de
	pop	hl
	jr	pbit
pinv:
	bit	7,(ix+7)
	jr	z,pdone
	push	hl
	push	de
	ld	hl,4180h
	ld	de,0
	call	fldiv
pdone:
	pop	ix
	pop	bc
	pop	bc
	pop	bc
	pop	bc
	ret

;	HLDE is y. Carry set means not an integer.
;	Carry clear and Z set means an even integer.
;	The value in HLDE is destroyed.

yodd:
	ld	a,h
	and	7fh
	sub	64
	jr	c,ynot
	jr	z,ynot
	cp	25
	jr	nc,yeven
	ld	b,a
	ld	a,24
	sub	b
	ld	b,a
	ld	c,0
	ld	a,b
	or	a
	jr	z,ypar
ysft:
	srl	l
	rr	d
	rr	e
	jr	nc,ystick
	inc	c
ystick:
	djnz	ysft
	ld	a,c
	or	a
	jr	nz,ynot
ypar:
	ld	a,e
	and	1
	ret

yeven:
	xor	a
	ret

ynot:
	scf
	ret

;	HLDE is y. Carry set means y is not an integer below 2^24.
;	Carry clear returns the absolute value in LDE.
;	H is 00h or 80h. The old HLDE value is destroyed.

yival:
	ld	b,h
	ld	a,h
	and	7fh
	sub	64
	jr	c,ynot
	jr	z,ynot
	cp	25
	jr	nc,ynot
	ld	c,a
	ld	a,24
	sub	c
	ld	c,a
	jr	z,yisgn
yish:
	srl	l
	rr	d
	rr	e
	jr	c,ynot
	dec	c
	jr	nz,yish
yisgn:
	ld	a,b
	and	80h
	ld	h,a
	or	a
	ret
