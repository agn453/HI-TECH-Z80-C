;	Set/get uid for CP/M

	global	_getuid, _setuid, csv, cret

	entry	equ	5		;CP/M system call entry
	sguid	equ	20h		;set/get uid call value
	arg	equ	6		;offset of 1st arg

	psect	text
_getuid:
	call	csv
	ld	c,sguid			;get/set uid code
	ld	e,0FFh			;to get rather than set
	push	ix
	call	entry
	pop	ix
	ld	l,a
	ld	h,0
	jp	cret

_setuid:
	call	csv
	ld	e,(ix+arg)		;get argument
	ld	c,sguid
	push	ix
	call	entry
	pop	ix
	jp	cret
