	global	_exit, __cpm_clean, bDos

	psect	text
_exit:
	call	__cpm_clean
        pop	hl ; return address 
        pop	hl ; exit status
	ld	(80h),hl	;store exit status
        ld	b,l
        ld	c,062h ; _TERM
        call	bDos	; should not return
	jp	0	; Warm boot CP/M
