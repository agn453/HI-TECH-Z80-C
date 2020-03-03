;	char bdos(func, arg)

	global	csv,cret

entry	equ	5		; CP/M entry point

arg	equ	8		;argument to call
func	equ	6		;desired function

	global	_bdos

	psect	text
_bdos:
	call	csv
	ld	e,(ix+arg)
	ld	d,(ix+arg+1)
	ld	c,(ix+func)
	push	ix
	push	iy
	call	entry
	pop	iy
	pop	ix
	ld	l,a
	rla
	sbc	a,a
	ld	h,a
	jp	cret
