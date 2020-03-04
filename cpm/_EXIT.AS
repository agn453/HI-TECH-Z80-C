EXITSTS	equ	80h		;where to store exit status [CP/M 2]

	global	__exit, __cpm_clean

	psect	text
__exit:
	call	__cpm_clean
	pop	hl		;return address
	ld	c,0ch
	call	5		;Get version
	cp	30h	      
	pop	hl		;exit status
	ld	(EXITSTS),hl
	jp	c,0		;Warm boot CP/M if < CP/M 3
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

	end
