; This is ZNRTCPM for the Z80 (with minimal getargs)

	psect	text,global,pure
	psect	data,global
	psect	bss,global

	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start,_main,_exit,__Hbss, __Lbss, __argc_, __z3env, startup
        global  __piped,__initrsx
	global	__getargs

start:		; DOS Protection	8080/Z80	x86
				;	---------  --------------------
	defb	0EBh,04h	;	EX DE,HL   JMPS LABE
				;	INC B

	ex	de,hl		;	EX DE,HL
	defb	0C3h		;	JP BEGIN
	defw	begin		; (We don't want the assembler optimising
				;  this to a relative branch)

labe:	defb	0B8h,0,09h	;		   MOV AX,9 ; print string

	defb	0BAh		;		   MOV DX,OFFSET BVMES
	defw	bvmes

	defb	0CDh,021h	;		   INT 21h  ; call DOS

	defb	0B8h,01h,4Ch	;		   MOV AX,4C01h ; terminate
				;		   ; with CTRL-BREAK code (1)

	defb	0CDh,021h	;		   INT 21h  ; call DOS
						   ; we never return here

	; Some text for viewing if typed

	defm	'Compiled with Hi-Tech C'
	defb	13,10,26

	psect	data
z3ecst:	defm	'Z3ENV'
bvmes:	defm	'This CP/M program requires a Z80 CPU.'
	defb	13,10,'$'
memmes:	defm	'Not enough memory to run.'
	defb	13,10,'$'
	psect	text

begin:	sub	a		;Stop 8080 processors running this.
	jp	po,okver

	ld	de,bvmes
	ld	c,9		;Print error message
	jp	5

;Check if HL holds a valid Z-System environment descriptor

okver:	ld	l,0		;Start of the page
	ld	a,(hl)
	cp	0c3h		;Must start with a jump
	jr	nz,noz3
	ld	l,1bh
	ld	a,(hl)		;Must refer to itself
	or	a
	jr	nz,noz3
	inc	hl
	ld	a,(hl)
	cp	h
	jr	nz,noz3
	ld	l,3		;Must contain the string "Z3ENV"
	ld	de,z3ecst
	ld	b,5		;Check 5 characters
z3ecmp:	ld	a,(de)
	cp	(hl)
	jr	nz,noz3
	inc	hl
	inc	de
	djnz	z3ecmp
	ld	l,0
	defb	3eh		;Swallow the LD HL
noz3:	ld	hl,0
yesz3:	ld	(__z3env),hl
	ld	hl,(6)		;Base of FDOS
	push	hl
	dec	h		;Allow 128 stack levels at least
	ld	de,__Hbss	;Last address used by the program
	and	a
	sbc	hl,de
	jr	nc,memok
	ld	de,memmes	;Bss segment hits the BDOS.
	ld	c,9
	call	5
	rst	0

memok:	
	ld	c,12
	call	5		;Check CP/M version
	ld	a,l
	cp	30h
	jr	nc,iscpm3
	xor	a		; CP/M 2.2 has no RSX
	jr	norsx

;If the PIPEMGR RSX is loaded, initialise it.

iscpm3:	ld	c,60		; RSX call
	ld	de,rsxpb	; RSX data
	call	5
	ld	a,h
	inc	l		;HL=00FFh if no PIPEMGR present.
	or	l
norsx:	ld	(__piped),a
	call	nz,__initrsx	;use PIPEMGR for stdin, stdout and stderr
        pop	hl		;base address of fdos
	ld	sp,hl		;stack grows downwards
	ld	de,__Lbss	;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc	
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory
	ld	hl,nularg
	push	hl
	ld	hl,80h		;argument buffer
	ld	c,(hl)
	inc	hl
	ld	b,0
	add	hl,bc
	ld	(hl),0		;zero terminate it
	ld	hl,81h
	push	hl
	call	startup
	pop	bc		;unjunk stack
	pop	bc
	ld	bc,(__z3env)	;main (argc,argv,z3env)
	push	bc
	push	hl
	ld	hl,(__argc_)
	push	hl
	call	_main
	push	hl
	call	_exit
	rst	0
;
; This is a substitute getargs() function that is of minimal
; size and does not perform wildcard argument expansion and
; redirection.
;

__getargs:
	pop	hl		;return address
	exx
	pop	hl
	pop	hl		;unjunk stack
	ld	a,(80h)		;get buffer length
	inc	a		;allow for null byte
	neg
	ld	l,a
	ld	h,-1
	add	hl,sp
	ld	sp,hl		;allow space for args
	ld	bc,0		;flag end of args
	push	bc
	ld	hl,80h		;address of argument buffer
	ld	c,(hl)
	ld	b,0
	add	hl,bc
	ld	b,c
	ex	de,hl
	ld	hl,(6)
	ld	c,1
	dec	hl
	ld	(hl),0
	inc	b
	jr	3f

2:	ld	a,(de)
	cp	' '
	dec	de
	jr	nz,1f
	push	hl
	inc	c
4:	ld	a,(de)		;remove extra spaces
	cp	' '
	jr	nz,5f
	dec	de
	jr	4b
5:
	xor	a
1:	dec	hl
	ld	(hl),a
3:
	djnz	2b
	ld	(__argc_),bc		;store argcount
	ld	hl,nularg
	push	hl
	ld	hl,0
	add	hl,sp
	exx
	push	de		;junk the stack again
	push	de
	push	hl		;return address
	exx
	ret

	psect	data
nularg:	defb	0
__z3env:
	defw	0
__piped:
        defw    0
rsxpb:	defb	79h,1
	defw	pipesgn
pipesgn:
	defm	'PIPEMGR '

	end	start
