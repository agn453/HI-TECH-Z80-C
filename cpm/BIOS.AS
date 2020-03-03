	psect	text

	global	_bios, csv,cret

arg	equ	6			;offset of 1st arg on stack

_bios:
	call	csv
	ld	hl,1f			;set up return address
	push	hl
	ld	l,(ix+arg+0)		;get bios index
	dec	l			;adjust
	ld	e,l
	ld	h,0
	ld	d,0
	add	hl,hl			;double it
	add	hl,de			;times 3
	ld	de,(1)			;get warm boot address
	add	hl,de			;now have transfer address
	push	hl			;put on stack
	ld	c,(ix+arg+2)		;get 1st arg
	ld	b,(ix+arg+3)
	ld	e,(ix+arg+4)		;get 2nd arg
	ld	d,(ix+arg+5)
	ret				;do bios call
1:
	ld	l,a			;return value in a
	ld	h,0
	jp	cret
