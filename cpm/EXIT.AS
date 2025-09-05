	global	_exit, __cpm_clean

BDOS	equ	00005h
CPMVERS	equ	12
CPMRCOD	equ	108		; CP/M+ Get/Set Error Return Code 

	psect	text
_exit:	ld	(80h),hl
	call	__cpm_clean
	ld	c,CPMVERS
	call	BDOS		;Get version
	dec	h		; set Z if MP/M
	jp	z,0		; Warm boot if MP/M
	cp	30h	      
	jp	c,0		;Warm boot CP/M if < CP/M 3
	ld	hl,(80h)
	ld	a,l		;For compatibility with conditionals system,
				;use 7-bit CP/M 3 error codes.
	and	7fh
	ld	e,a
	ld	a,h		;Set D=0FFh for error
	or	l		;or 0 for all clear.
	jr	z,exit0
	ld	a,0ffh
exit0:	ld	d,a
	ld	c,CPMRCOD
	call	BDOS		;Report error.
	rst	0
