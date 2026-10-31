	psect	text

;	double	frndint(val)
;	double	val;

;	Round the argument to an integral value, return as a double

	global	__frndint, altof, ftol

__frndint:
	pop	bc	;return addr
	pop	de
	pop	hl	;float val
	push	hl
	push	de
	push	bc
	call	ftol
	jp	altof
