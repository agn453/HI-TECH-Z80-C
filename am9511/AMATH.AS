;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Functions that MATH.H does not name.
;	pi is the chip PUPI opcode.
;	log2 is ln(x) / ln(2).
;	exp2 is exp(x * ln(2)).
;	exp10 is exp(x * ln(10)).
;	A non-positive log2 argument returns 0.

	global	_pi, _log2, _exp2, _exp10
	global	pushf, popf

	psect	text

APUCMD	equ	43h
OP_LN	equ	09h
OP_EXP	equ	0ah
OP_MUL	equ	12h
OP_DIV	equ	13h
OP_PI	equ	1ah

_pi:
	ld	a,OP_PI
	out	(APUCMD),a
	jp	popf

_log2:
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	call	pushf
	ld	a,OP_LN
	out	(APUCMD),a
	call	popf
	ld	a,l
	or	d
	or	e
	ret	z
	call	pushf
	ld	hl,ln2
	call	ldconst
	call	pushf
	ld	a,OP_DIV
	out	(APUCMD),a
	jp	popf

_exp2:
	ld	hl,ln2
	jr	expmul

_exp10:
	ld	hl,ln10

;	HL holds the address of the scale constant.
;	The C argument stays on the stack.
;	A zero product from a magnitude at or above 1 is an overflow.
;	exp of that product would be exp(0), which is 1.
;	A zero product from a smaller x is an underflow. exp(0) is correct.

expmul:
	push	hl
	ld	hl,4
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	ld	a,h
	and	7fh
	cp	41h
	sbc	a,a
	pop	bc
	push	af
	push	bc
	call	pushf
	pop	hl
	call	ldconst
	call	pushf
	ld	a,OP_MUL
	out	(APUCMD),a
	call	popf
	pop	af
	ld	b,a
	ld	a,l
	or	d
	or	e
	jr	nz,expgo
	ld	a,b
	or	a
	jr	nz,expgo
	ret
expgo:
	call	pushf
	ld	a,OP_EXP
	out	(APUCMD),a
	jp	popf

;	Load the Hi-Tech float at HL into HLDE.

ldconst:
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	ret

ln2:	deff	0.6931471805599453
ln10:	deff	2.3025850929940457
