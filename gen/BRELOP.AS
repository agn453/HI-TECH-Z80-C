;	byte relational	operation - returns flags correctly for
;	comparision of words in a and b

	psect	text
	global	brelop

brelop:
	push	de
	ld	e,a
	xor	b		;compare signs
	jp	m,1f		;if different, return sign of lhs
	ld	a,e
	sbc	a,b
	pop	de
	ret
1:
	ld	a,e		;get sign of lhs
	and	80h		;mask out sign flag
	ld	d,a
	ld	a,e
	sbc	a,b		;set carry flag if appropriate
	ld	a,d
	inc	a		;set sign flag as appropriate and reset Z flag
	pop	de
	ret
