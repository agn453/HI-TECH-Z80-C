	global	_exit, __cpm_clean

	psect	text
_exit:	ld	(80h),hl
	call	__cpm_clean
	ld	c,0ch
	call	5		;Get version
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
	ld	c,6ch
	call	5		;Report error.
	rst	0
