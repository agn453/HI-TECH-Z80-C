EXITSTS	equ	80h		;where to store exit status

	global	__exit, __cpm_clean

	psect	text
__exit:
	call	__cpm_clean
	pop	hl		;return address
	pop	hl		;exit status
	ld	(EXITSTS),hl
	jp	0		;Warm boot CP/M
