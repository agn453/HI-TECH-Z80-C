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
	global	pushf, popf, pushl, popl, cunary, apwait

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

;	HLDE minus the stack operand.
;	FSUB is NOS minus TOS. Push HLDE first and the operand second.
;	A zero right operand returns HLDE. A zero left operand returns
;	the negated right operand. The negate is a sign flip, not a chip trip.

flsub:
	ld	a,l
	or	d
	or	e
	exx
	pop	bc
	pop	de
	pop	hl
	push	bc
	jr	z,fneg
	ld	a,l
	or	d
	or	e
	exx
	ret	z
	ld	a,FSUB
	jp	fboth
fneg:
	ld	a,h
	or	a
	jr	z,flzero
	xor	80h
	ld	h,a
	ret

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
	ld	a,FADD
	jp	fboth

;	Multiply HLDE by the stack operand. A zero mantissa returns 0.

flmul:
	ld	c,FMUL
	jr	fzero

;	Divide HLDE by the stack operand.
;	A zero dividend or a zero divisor returns 0.

fldiv:
	ld	c,FDIV
fzero:
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
	ld	a,c
	jr	fboth

flzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

;	Both operands are nonzero. A holds the opcode.
;	Main HLDE is the left value. Alternate HLDE is the right value.

fboth:
	push	af
	call	pushf
	exx
	call	pushf
	exx
	pop	af
	out	(APUCMD),a
	jp	popf

;	C double in the first stack argument. A holds the opcode.
;	The caller stack stays intact.

cunary:
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	push	af
	call	pushf
	pop	af
	out	(APUCMD),a
	jp	popf

;	Idle test. Carry is clear when BUSY is clear.
;	IN A,(n) is 11 states, RLCA is 4, RET NC taken is 11. Total 26.
;	The fast path pushes no registers. A is rotated. Do not use A.
;	A missing chip sets carry and returns A = FFh.

apwait:
	in	a,(APUCMD)
	rlca
	ret	nc
	push	bc
	ld	bc,0
apw1:
	in	a,(APUCMD)
	rlca
	jr	nc,apw0
	dec	bc
	ld	a,b
	or	c
	jr	nz,apw1
	pop	bc
	scf
	ld	a,0ffh
	ret
apw0:
	pop	bc
	or	a
	ret

;	Push HLDE. Low byte first. XOR 40h on the exponent byte.
;	OUT (C),r is 12 states. LD A,r plus OUT (n),A is 15 states.
;	Data writes do not set BUSY. The caller waits on the command.

pushf:
	ld	a,l
	or	d
	or	e
	jr	z,push0
	ld	bc,APUDAT
	out	(c),e
	out	(c),d
	out	(c),l
	ld	a,h
	xor	40h
	out	(c),a
	ret
push0:
	ld	bc,APUDAT
	xor	a
	out	(c),a
	out	(c),a
	out	(c),a
	out	(c),a
	ret

;	Push a 32-bit integer. No exponent XOR.

pushl:
	ld	bc,APUDAT
	out	(c),e
	out	(c),d
	out	(c),l
	out	(c),h
	ret

;	Pop a float into HLDE. High byte first.
;	IN r,(C) is 12 states. An error code or a timeout returns 0.

popf:
	call	apwait
	jp	c,fpzero
	ld	bc,APUDAT
	in	h,(c)
	in	l,(c)
	in	d,(c)
	in	e,(c)
	inc	c
	in	a,(c)
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

;	Pop a 32-bit integer. An error code or a timeout returns 0.

popl:
	call	apwait
	jp	c,fpzero
	ld	bc,APUDAT
	in	h,(c)
	in	l,(c)
	in	d,(c)
	in	e,(c)
	inc	c
	in	a,(c)
	and	1eh
	ret	z
	jp	fpovrflw

	psect	bss
fperr:
	defs	1
	psect	text
