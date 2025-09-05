EXITSTS	equ	80h		;where to store exit status [CP/M 2]

BDOS	equ	00005h

CPMVERS	equ	12		; Get CP/M Version number
CPMRCOD	equ	108		; CP/M+ Get/Set Error Return Code 

	global	__exit, __cpm_clean

	psect	text
__exit:
	call	__cpm_clean
	pop	hl		;return address
	ld	c,CPMVERS
	call	BDOS		;Get version
	dec	h		;sets Z flag if MP/M
	pop	hl		;exit status
	jp	z,0		;Warm boot if MP/M
	cp	30h	      
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
	ld	c,CPMRCOD
	call	BDOS		;Report error.
	rst	0

	end
