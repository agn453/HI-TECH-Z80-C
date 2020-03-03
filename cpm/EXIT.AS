	global	_exit, __cpm_cle

	psect	text
_exit:
	ld	(80h),hl	;store exit status
	call	__cpm_cle
	jp	0		;Warm boot CP/M
