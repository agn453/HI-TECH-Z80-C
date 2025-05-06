;	Shift operations - the count is always in B,
;	the quantity to be shifted is in HL, except for the assignment
;	type operations, when it is in the memory location pointed to by
;	HL

	global	shar	;shift arithmetic right
	psect	text


shar:
	ld	a,b		;check for zero shift
	or	a
	ret	z
	cp	16		;16 bits is maximum shift
	jr	c,1f		;is ok
	ld	b,16
1:
	sra	h
	rr	l
	djnz	1b
	ret
