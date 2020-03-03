;	Conversion of integer type things to floating. Uses routines out
;	of float.as.

	psect	text

	global	altof, lltof, aitof, litof, abtof, lbtof
	global	fpnorm

lbtof:
	ld	e,a
	ld	d,0
litof:
	ex	de,hl		;put arg in de
	ld	l,0		;zero top byte
b3tof:
	ld	h,64+24
	jp	fpnorm

abtof:
	ld	e,a
	rla
	sbc	a,a
	ld	d,a

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
	ld	e,d		;shift down 8 bits
	ld	d,l
	ld	l,h
	ld	h,64+24+8	;the 8 compensates for the shift
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
