*Title	Indirect command execution
*Head	Description

;	Copyright (C) 1984 HI-TECH SOFTWARE

;This is the indirect command processor
;It is invoked with one argument which is the name of an indirect
;command file, normally created by the C command or a similar
;command processor, and executes the commands in the file. See
;exec.doc for a description of the command syntax in the file.


*Heading	Command values and BDOS calls
BASE	equ	80h		;base of command values

EXIT	equ	BASE+0		;Exit command file
EXEC	equ	BASE+1		;Execute a transient command
IGN_ERR	equ	BASE+2		;Ignore errors
DEF_ERR	equ	BASE+3		;Exit on error
SKP_ERR	equ	BASE+4		;Skip on error to next TRAP
TRAP	equ	BASE+5		;Trap for error skips
IF_ERR	equ	BASE+6		;Execute the next command if any error
IF_NERR	equ	BASE+7		;Execute the next command if no error
ECHO	equ	BASE+8		;Echo all EXEC commands 
PRINT	equ	BASE+9		;Print buffer on console
RM_FILE	equ	BASE+10		;Remove file
RM_EXIT	equ	BASE+11		;Remove file and exit

WBOOT	equ	0		;Address for warm boot
ENTRY	equ	5		;Bdos entry
DFCB	equ	5CH		;Default FCB
DBUF	equ	80H		;Default buffer
TPA	equ	100H		;Transient area
SECSIZE	equ	128		;CP/M record size
FCBSIZ	equ	33		;Size of non-random FCB

CONIN	equ	1		;Console get char
CONOUT	equ	2		;Console out char
CONST	equ	11		;Get console status
RDSK	equ	13		;reset disk system
SELDSK	equ	14		;select drive
OPEN	equ	15		;Open file
DELETE	equ	19		;Delete file
READ	equ	20		;Read record
GETLOG	equ	25		;Get current disk
SDMA	equ	26		;Set DMA address
UID	equ	32		;Set/get user ID

	macro	bdos,fun,arg	;Macro for bdos calls
	ld	de,arg
	ld	c,fun
	call	ENTRY
	endm

	macro	bdosx,fun	;bdos calls with no arg
	ld	c,fun
	call	ENTRY
	endm

CR	equ	0DH		;Carriage return
LF	equ	0AH		;Line feed

*Head	Main program
	psect	text

	global	__Lbss

	defs	TPA		;relocate to start of TPA
start:
	ld	sp,(ENTRY+1)	;Set up stack pointer
	bdosx	GETLOG
	ld	(curdsk),a
	bdos	UID, 0FFh
	ld	(curuid),a
	ld	hl,DFCB
	ld	de,cfcb
	ld	bc,16
	ldir			;set up command FCB
	bdos	OPEN,cfcb
	inc	a
	jr	nz,1f
	ld	hl,nofile
	call	print
	jp	doexit
1:
	ld	hl,(ENTRY+1)	;top of code after relocation
	ld	de,stack	;top before relocation
	scf			;set carry flag
	sbc	hl,de		;gives distance
	ex	de,hl
	ld	bc,(__Lbss)	;reloc count
	ld	hl,__Lbss+2	;start of addresses
1:
	ld	a,c
	or	b		;check for count zero
	jr	z,3f		;start if so
	dec	bc
	ld	a,(hl)		;get address
	inc	hl
	push	hl		;save it
	ld	h,(hl)
	ld	l,a
	push	hl		;save it
	push	de		;and it
	ld	de,5f		;check for legal modify
	or	a
	sbc	hl,de
	pop	de		;restore regs
	pop	hl
	jr	c,4f
	ld	a,(hl)		;get value
	inc	hl
	push	hl
	ld	h,(hl)
	ld	l,a
	add	hl,de		;add difference
	ex	de,hl		;put in de
	ex	(sp),hl		;restore pointer
	ld	(hl),d		;store new value
	dec	hl
	ld	(hl),e
	pop	de		;restore difference
4:
	pop	hl		;restore list pointer
	inc	hl		;point to next
	jr	1b
3:
	ld	hl,stack-1
	add	hl,de		;add in difference
	ex	de,hl		;put dest in de
	ld	hl,stack-1	;source in hl
	ld	bc,stack-cmdstart	;amount to move
	lddr
5:
	jp	cmdstart	;jump to it

cmdstart:
	ld	hl,(ENTRY+1)
	ld	(cmdstart-2),hl
	ld	a,jp
	ld	(cmdstart-3),a
	ld	hl,cmdstart-3
	ld	(ENTRY+1),hl
	ld	hl,(WBOOT+1)
	ld	(bootsave),hl
	ld	hl,loop
	ld	(WBOOT+1),hl
	ld	hl,0
	ld	(DBUF),hl	;initially no error
loop:
	ld	sp,stack
	ld	hl,(DBUF)
	ld	a,h
	or	l
	jr	z,1f		;no error, nothing required
	ld	(errors),a	;update flag
	ld	a,(erract)
	dec	a		;set flags		;default action?
	jp	m,doexit	;default, exit pronto
	jr	z,1f		;ignore errors
	call	skipto		;skip to next trap command
1:
	ld	hl,0
	ld	(DBUF),hl
	call	docmd
	jr	loop

docmd:
	call	getch
	sub	BASE		;Check for legal range
	jp	c,cerr		;too small
	cp	NCMDS		;Check other end of range
	jp	nc,cerr
	ld	hl,ctable
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	push	hl		;push the address
	call	getch
	ld	(length),a	;store the length
	ret			;dispatch to the routine

ctable:
	defw	doexit		;EXIT
	defw	doexec		;EXEC
	defw	ignerr		;IGN_ERR
	defw	deferr		;DEF_ERR
	defw	skperr		;SKPERR
	defw	trap		;TRAP
	defw	iferr		;IF_ERR
	defw	ifnerr		;IF_NERR
	defw	echo		;ECHO
	defw	xprint		;PRINT
	defw	rm		;RM_FILE
	defw	rm_exit		;RM_EXIT

NCMDS	equ	$-ctable	;number of legal commands

cerr:	ld	hl,errms
	call	print	
	jp	doexit

errms:	defb	7
	defm	'Fmt err'

doexit:
	ld	hl,(bootsave)
	ld	(WBOOT+1),hl
	jp	(hl)

doexec:
	ld	hl,DFCB
	ld	de,DFCB+1
	ld	bc,FCBSIZ-1
	ld	(hl),0
	ldir
	call	getch
	ld	(newuid),a
	ld	hl,DFCB
	ld	b,16
	call	rdbytes
9:
	ld	a,(echflg)
	or	a
	jr	z,2f
	ld	a,(newuid)
	inc	a
	jr	z,4f
	add	a,'0'-1
	call	putch
	ld	a,':'
	call	putch
4:
	ld	a,(DFCB)
	or	a
	jr	z,4f
	add	a,'A'-1
	call	putch
	ld	a,':'
	call	putch
4:
	ld	hl,DFCB+1
	ld	b,8
1:
	ld	a,(hl)
	cp	' '
	jr	z,2f
	push	hl
	push	bc
	call	putch
	pop	bc
	pop	hl
	inc	hl
	djnz	1b
2:
	ld	a,(newuid)
	ld	e,a
	bdosx	UID
	bdos	OPEN,DFCB	;open the file
	inc	a		;-1 is error on return
	jr	nz,1f		;skip if found
filerr:
	ld	a,(curuid)
	ld	e,a
	bdosx	UID
	ld	hl,notfnd
	call	print
	bdosx	CONIN
	bdosx	RDSK
	bdos	SELDSK,(curdsk)
	jr	9b

1:
	ld	de,TPA		;read into transient area
1:
	push	de		;save ptr
	bdosx	SDMA		;set buffer address
	bdos	READ,DFCB	;read sector
	pop	hl		;pop ptr
	ld	de,SECSIZE
	add	hl,de
	ex	de,hl
	or	a
	jr	z,1b		;loop if more

	ld	a,(curuid)	;restore uid
	ld	e,a
	bdosx	UID
	ld	hl,DFCB
	ld	b,32
	call	rdbytes

	call	getch
	ld	hl,DBUF
	ld	(hl),a
	inc	hl
	ld	b,a
	call	rdbytes

	ld	a,(echflg)
	or	a
	jr	z,1f
	ld	hl,DBUF		;just print for now
	call	print
1:
	jp	TPA		;execute program

rdbytes:
	call	getch
	ld	(hl),a
	inc	hl
	djnz	rdbytes
	ret

xprint:
	ld	b,a		;a has the length already
	ld	hl,TPA
	ld	(hl),a
	inc	hl
	call	rdbytes
	ld	hl,TPA

print:
	ld	b,(hl)
	inc	hl
1:
	ld	a,(hl)
	inc	hl
	push	hl
	push	bc
	call	putch
	pop	bc
	pop	hl
	djnz	1b
	ld	a,CR
	call	putch
	ld	a,LF
putch:
	ld	e,a
	bdosx	CONOUT
	ret

ignerr:
	ld	a,1
1:
	ld	(erract),a
	ret

deferr:
	xor	a
	jr	1b

skperr:
	ld	a,2
	jr	1b

skip:
	call	getch		;get the command byte
	push	af		;save it
	call	getch		;length
	ld	b,a		;put into counter
	ld	hl,TPA		;s suitable place to read into
	or	a		;check for zero count
	call	nz,rdbytes	;read if any to read
	pop	af
	ret			;return with command byte in A


;	Skip up to the next trap

skipto:
	call	skip
	cp	EXIT		;End?
	jp	z,doexit	;yes, return to CP/M
	cp	TRAP		;got a trap yet?
	jr	nz,skipto	;loop for more
	ret

rm:
	ld	b,16
	ld	hl,DFCB
	call	rdbytes
	bdos	DELETE,DFCB
	ret

rm_exit:
	call	rm
	jp	doexit

getch:
	push	hl
	ld	hl,(cptr)
	ld	de,cbuf+SECSIZE
	or	a
	sbc	hl,de
	jr	nz,1f
	push	bc
	bdos	SDMA,cbuf
	bdos	READ,cfcb
	pop	bc
	or	a
	jp	nz,doexit		;exit on EOF
	ld	hl,cbuf
	ld	(cptr),hl
1:
	ld	hl,(cptr)
	ld	a,(hl)
	inc	hl
	ld	(cptr),hl
	pop	hl
	ret

trap:
	ret

echo:
	ld	a,1
	ld	(echflg),a
	ret

iferr:
	ld	a,(errors)	;check for errors
	or	a		;set flags
	jp	z,skip		;skip next command if no error
	ret

ifnerr:
	ld	a,(errors)
	or	a		;any error?
	jp	nz,skip		;skip if not
	ret

notfnd:
	defb	40
	defm	': Not found - change disks, hit a key'
	defb	7,0Dh,0Ah
nofile:
	defb	7
	defm	'No file'

*Heading	Data areas

cfcb:	defs	FCBSIZ		;Command file FCB
cbuf:	defs	SECSIZE		;Command file buffer
cptr:	defw	cbuf+SECSIZE	;Pointer to command input
errst:	defb	0		;error status
erract:	defb	0		;action on error
echflg:	defb	0
curdsk:	defs	1
length:
	defs	1		;command length
errors:
	defs	1		;cumulative error flag
curuid:
	defs	1		;current uid
newuid:
	defs	1		;user id for EXEC command
bootsave:
	defs	2
	defs	40		;small stack
stack:

	psect	bss		;for reloc info
	end	start
