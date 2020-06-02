;	Offsets of things in the _iob structure

	ptr	equ	0		;pointer to next byte
	cnt	equ	2		;number of bytes left
	base	equ	4		;beginning of buffer
	flag	equ	6		;flag bits
	file	equ	8		;file number

;	The bit numbers of the flags in flag
	
	_IOREAD_BIT	equ	0
	_IOWRT_BIT	equ	1
	_IONBF_BIT	equ	2
	_IOMYBUF_BIT	equ	3
	_IOEOF_BIT	equ	4
	_IOERR_BIT	equ	5
	_IOSTRG_BIT	equ	6
	_IOBINARY_BIT	equ	7

;	Various characters

	CPMEOF	equ	032q		; EOF byte 
	NEWLINE	equ	012q		; newline character 
	RETURN	equ	015q		; carriage return 

	EOF	equ	-1		; stdio EOF value

