;	arithmetic long left shift
;	value in HLDE, count in B

	global	allsh, lllsh
	psect	text

allsh:
lllsh:
	ld	a,b		;check for zero shift
	or	a
	ret	z
	cp	33
	jr	c,1f		;limit shift to 32 bits
	ld	b,32
1:
	ex	de,hl
	add	hl,hl
	ex	de,hl 
	adc	hl,hl
	djnz	1b
	ret
