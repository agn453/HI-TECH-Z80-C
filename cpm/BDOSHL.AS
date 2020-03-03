;	Bdos calls which return values in HL

;	short	bdoshl(fun, arg);

	psect	text

entry	equ	5		; CP/M entry point

arg	equ	8		;argument to call
func	equ	6		;desired function


	global	_bdoshl
	global	csv,cret
	psect	text
_bdoshl:
	call	csv
	ld	e,(ix+arg)
	ld	d,(ix+arg+1)
	ld	c,(ix+func)
	push	ix
	call	entry
	pop	ix
	jp	cret		;return value already in hl
