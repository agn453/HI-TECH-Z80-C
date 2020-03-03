;	long	_fbcd(x, exp, buf)
;	double	x;
;	int *	exp;
;	char *	buf;

;	split x into mantissa and decimal exponent parts
;	return value is the (long) mantissa part, exponent part is
;	stored in *exp as two's complement. Mantissa is stored into buf
;	as an ascii string.

argx	equ	6		;offset of x argument
argexp	equ	10		;offset of exp argument
argbuf	equ	12		;offset of buf argument
exptmp	equ	-1		;offset of exp temp byte
sgntmp	equ	-2		;offset of sign byte

NDIG	equ	8		;number of decimal digits

	global	__fbcd, rcsv, cret, flmul, negmant,lldiv,llmod
	psect	text

hasfrac:
	ld	c,0		;zero number
	ld	a,e		;check low 8 bits
	or	a
	jr	nz,1f		;non zero bit in low 8 bits
	ld	c,8		;bump count
	ld	a,d		;check next 8 bits
	or	a		;is there a bit there?
	jr	nz,1f		;yup
	ld	c,16
	ld	a,h		;now check next 8 bits
1:
	rra			;shift bottom bit out
	jr	c,2f		;found a bit!
	inc	c		;increment count
	jr	1b		;and loop

2:
	ld	a,h		;get exponent
	res	7,a		;clear sign bit - should be zero anyway
	sub	64+24		;normalize - remove bias
	add	a,c		;add in bit position
	ret			;return with value in a and flags set

__fbcd:
	call	rcsv		;get x into hlde, exp into bc
	dec	sp		;make room for exponent
	dec	sp		;and sign flag
	xor	a
	ld	(ix+exptmp),a	;zero it
	ld	(ix+sgntmp),a
	ld	(bc),a		;and the returned exp value
	ex	de,hl		;put hi word into hl as required
	ld	a,h		;check for zero exponent
	and	7Fh		;zero exponent means 0.0
	jp	nz,1f		;return if x == 0.0
	ld	l,a		;zero mantissa just in case
	ld	e,a
	ld	d,a
	ld	h,a		;and sign/exponent
	jp	sbcd		;return with mantissa = 0, exponent = 0
1:
	res	7,h		;test mantissa sign
2:
	call	hasfrac		;any fractional part?
	jp	m,1f		;negative if there is fractional part
	push	hl		;put x on stack
	push	de
	ld	hl,(tenth+2)
	ld	de,(tenth)
	call	flmul		;returns with value in hlde
	inc	(ix+exptmp)	;increment exponent
	jr	2b		;now check again
1:
	push	hl
	push	de		;pass x as argument
	ld	hl,(ten+2)
	ld	de,(ten)
	call	flmul		;multiply it
	dec	(ix+exptmp)	;and decrement exponent
	call	hasfrac		;check for fractional part
	jp	m,1b		;loop if still fractional
	ld	a,h		;get exponent
	ld	h,0		;zero top byte
	sub	64+24		;offset exponent
2:
	or	a		;check for zero
	jr	z,3f	;return if finished
	jp	p,4f
	srl	l		;shift l down
	rr	d		;rotate the rest
	rr	e
	inc	a		;increment count
	jr	2b

4:
	sla	e
	rl	d
	rl	l
	rl	h
	dec	a
	jr	2b

3:
	ld	a,(ix+exptmp)
	ld	c,(ix+argexp)	;get exp pointer
	ld	b,(ix+argexp+1)
	ld	(bc),a		;store exponent
	inc	bc
	rla
	sbc	a,a
	ld	(bc),a		;sign extend it
	bit	0,(ix+sgntmp)	;test sign
	jp	z,sbcd		;return if no negation needed
	push	hl		;push hi word
	ld	hl,0		;get a zero
	or	a		;reset carry
	sbc	hl,de		;do low subtraction
	ex	de,hl		;put into de again
	ld	hl,0		;get another zero
	pop	bc		;get hi word
	sbc	hl,bc		;subtract again

sbcd:				;now store as ascii
	ld	c,(ix+argbuf)
	ld	b,(ix+argbuf+1)
	push	bc
	pop	iy
	ld	bc,NDIG
	add	iy,bc		;point to end of buffer
	ld	b,c
	ld	(iy+0),0	;null terminate
	push	hl
	push	de		;save return value
1:
	push	bc		;save count
	push	hl		;save value
	push	de
	ld	bc,0
	push	bc		;pass 10 on stack
	ld	bc,10
	push	bc
	call	llmod
	ld	a,e		;get remainder
	add	a,'0'		;asciize
	dec	iy
	ld	(iy+0),a
	pop	de
	pop	hl		;restore value
	ld	bc,0		;now divide by 10
	push	bc
	ld	bc,10
	push	bc
	call	lldiv
	pop	bc		;restore count
	djnz	1b		;loop if more to do
	pop	de
	pop	hl
	jp	cret		;all done
	


	psect	data
ten:	deff	10.0
tenth:	deff	0.1
