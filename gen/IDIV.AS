;	16 bit divide and modulus routines

;	called with dividend in hl and divisor in de

;	returns with result in hl.

;	adiv (amod) is signed divide (modulus), ldiv (lmod) is unsigned

	global	adiv,ldiv,amod,lmod
	psect	text

amod:
	call	adiv
	ex	de,hl		;put modulus in hl
	ret

lmod:
	call	ldiv
	ex	de,hl
	ret

ldiv:
	xor	a
	push	af
	ex	de,hl
	jr	dv1


adiv:
	ld	a,h
	xor	d		;set sign flag for quotient
	ld	a,h		;get sign of dividend
	push	af
	call	negif
	ex	de,hl
	call	negif
dv1:	ld	b,1
	ld	a,h
	or	l
	jr	nz,dv8
	pop	af
	ret

dv8:	push	hl
	add	hl,hl
	jr	c,dv2
	ld	a,d
	cp	h
	jr	c,dv2
	jp	nz,dv6
	ld	a,e
	cp	l
	jr	c,dv2
dv6:	pop	af
	inc	b
	jp	dv8

dv2:	pop	hl
	ex	de,hl
	push	hl
	ld	hl,0
	ex	(sp),hl

dv4:	ld	a,h
	cp	d
	jr	c,dv3
	jp	nz,dv5
	ld	a,l
	cp	e
	jr	c,dv3

dv5:	sbc	hl,de
dv3:	ex	(sp),hl
	ccf
	adc	hl,hl
	srl	d
	rr	e
	ex	(sp),hl
	djnz	dv4
	pop	de
	ex	de,hl
	pop	af
	call	m,negat
	ex	de,hl
	or	a			;test remainder sign bit
	call	m,negat
	ex	de,hl
	ret

negif:	bit	7,h
	ret	z
negat:	ld	b,h
	ld	c,l
	ld	hl,0
	or	a
	sbc	hl,bc
	ret
