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
;	The chip PWR opcode sees a positive base only.

	global	_exp, _pow, cunary, pushf, popf

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

;	x and y are already in the two register banks.
;	Push x, then y. PWR is NOS to the power TOS.

ppwr:
	call	pushf
	exx
	call	pushf
	exx
	ld	a,OP_PWR
	out	(APUCMD),a
	jp	popf

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
