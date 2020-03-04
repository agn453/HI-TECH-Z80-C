;	Set/get uid for CP/M

	global	_getuid, _setuid, csv, cret

	entry	equ	5		;CP/M system call entry
	sguid	equ	32		;set/get user number
	arg	equ	6		;offset of 1st arg

	psect	text
_getuid:
	call	csv
	ld	e,0FFh			;to get rather than set
	jr	4f			;Go to common code
_setuid:
	call	csv
	ld	e,(ix+arg)		;get argument
4:
	ld	c,sguid
	push	ix
	call	entry
	pop	ix
	jp	cret
