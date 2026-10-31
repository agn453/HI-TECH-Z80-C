;	Return value of the stack pointer

	psect	text
	global	__getsp
__getsp:
	ld	hl,0
	add	hl,sp
	ret
