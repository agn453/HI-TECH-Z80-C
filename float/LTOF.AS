;	Conversion of integer type things to floating. Uses routines out
;	of float.as.

	psect	text

	global	altof, lltof, aitof, litof, abtof, lbtof
	global	fpnorm

lbtof:
	ld	l,a
	ld	h,0
litof:
	ex	de,hl		;put arg in de
	ld	l,0		;zero top byte
b3tof:
	ld	h,64+24
	jp	fpnorm

abtof:
	ld	l,a
	rla
	sbc	a,a
	ld	h,a

aitof:
	bit	7,h		;negative?
	jp	z,litof		;no, treat as unsigned
	ex	de,hl
	ld	hl,0
	or	a
	sbc	hl,de		;negate it
	call	litof
	set	7,h		;set sign flag
	ret

lltof:
	ld	a,h		;anything in top byte?
	or	a
	jr	z,b3tof		;no, just do 3 bytes
	ld	c,0
;	Do it the hard way
slop:
	ld	a,h
	cp	1		;last shift coming up?
	jr	z,slop2
	srl	h
	rr	l
	rr	d
	rr	e
	inc	c
	jr	slop
slop2:
	inc	e
	jr	nz,slop3
	inc	d
	jr	nz,slop3
	inc	l
	jr	nz,slop3
	inc	h
slop3:
	srl	h
	rr	l
	rr	d
	rr	e
	ld	a,h
	or	a
	jr	z,slop4
	srl	h
	rr	l
	rr	d
	rr	e
	inc	c
slop4:
	ld	a,c
	add	a,64+25
	ld	h,a
	jp	fpnorm		;and normalize it

altof:
	bit	7,h		;negative?
	jr	z,lltof		;no, treat as unsigned
	push	hl		;negate it now
	ld	hl,0
	or	a
	sbc	hl,de
	ex	de,hl
	pop	bc
	ld	hl,0
	sbc	hl,bc
	call	lltof
	set	7,h		;set sign flag
	ret
