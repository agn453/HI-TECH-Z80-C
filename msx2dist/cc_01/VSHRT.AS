	psect	text,global,pure
	psect	data,global
	psect	bss,global

	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start,_main,_exit,__Hbss, __Lbss, __argc_, startup, bDos
; get value for mypages:
*include mypages.as 
; mypages equ	PAGES

start:
; store data vram code, sp must be in page 3 to use extbio
hokvld	equ	0FB20h
extbio	equ	0FFCAh
global	indir

; check dos version

	ld	c, 6Fh ; _DOSVER
	call	bDos
	or	a
	jp      nz,nodos2
	ld	a,b
	cp	2
	jp	c,nodos2

	ld	hl,03F00h
	add	hl,sp
	jr	nc,noram	; fail if sp < 0C100h
	ld	a,(hokvld)
	and	01h
	jr	z,noram
	ld	de, 0401h ; mapper support, get variable table address
	xor	a
	call	extbio
	ld	(ramsltp),a
	inc	e ; [mapper support], get jump table address
	call	extbio
	ld	de,018h ; offset for put_p0
	add	hl,de
	ld	de,3	; sizeof(jp nn)
	ld	(putp0p),hl
	add	hl,de
	call	indir	; get_p0
	ld	(map0p),a ; mapper segment for page 0
	add	hl,de
	ld	(putp1p),hl
	add	hl,de
	call	indir	; get_p1
	ld	(map1p),a ; mapper segment for page 1
	add	hl,de
	ld	(putp2p),hl
	add	hl,de
	call	indir	; get_p2
	ld	(map2p),a ; mapper segment for page 2

; move stack to keep everything we need to restore in <mypages> pages
	ld	sp,mypages * 4000h
; the rest is standard
	ld	de,__Lbss	;Start of BSS segment
	scf			;set carry
	ld	hl,__Hbss
	sbc	hl,de		;<size of uninitialized data area>-1
	ld	c,l
	ld	b,h
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
	push	hl
	ld	hl,(__argc_)
	push	hl
	call	_main
	push	hl
	call	_exit
	jp	0

	psect	data
nularg:	defb	0

	psect	text
noram:
	ld	de,noramm
	ld	hl,0DEh ; .NORAM
	push	hl
	jr	1f
nodos2:
	ld	de,nodos2m
	ld	hl,085h ; .BADVER
	push	hl
1:
	ld	c,9 ; _STROUT
	call	bDos
	jp	_exit

	psect	data
noramm:
	defm	'*** Not enough memory; need 48KiB in TPA'
	defb	13,10,'$'
nodos2m:
	defm	'*** Wrong version of MSX-DOS'
	defb	13,10,'$'


;De onderstaande code is gebaseerd op code uit MCCM 65, pp 12--15
;ipv MemMan wordt echter video RAM (en evt EXT. RAM) gebruikt

; int _spawn (char * program, char *args, char fds[3])

 	psect	text
__spawn: 
global csv, cret, indir, __spawn, _errno
 	call	csv

        ld	e,(ix+6)
        ld	d,(ix+7) ; program
	push	ix
	ld	bc,0640h ; _FFIRST, attr hidden/system
	ld	ix,80h
	call	bDos ; DOS2
	jr	z,1f
	pop	ix
	ld	(_errno),a
	ld	hl,-1
	jp cret
1:
        ld	a,(ix+25) ; drive nr.
	add	a,'A'-1		
        ld	hl,mypages*4000h
	ld	(hl),a
	inc	hl
	ld	(hl),':'
	inc	hl
	ld	(hl),'\'
	inc	hl
	ex	de,hl
	ld	c,5Eh ; _WPATH
	call	bDos ; DOS2
	dec	de
	dec	de
	dec	de
	ld	hl, program
	ld	c,6Ch ; _SENV
	call	bDos ; DOS2
	pop	ix
        ld	hl,81h
        ld	(hl),' '
        inc	hl
        ex	de,hl
        ld	l,(ix+8)
        ld	h,(ix+9) ; args
        ld	bc,7eh
12: 
        ld	a,(hl)
        ldi
        jp	po, 11f
        or	a
        jr	nz, 12b
11: 
        dec	de
 	xor	a
 	ld	(de),a
        ld	a,e
        sub	81h
        ld	(80h),a
	ld	(lenp),a
	
        ld	de,81h
	ld	hl,param
	ld	c,6Ch	; _SENV
	call	bDos 	; DOS2

	psect	data
program:
	defm	'PROGRAM'
	defb	0
param:
	defm	'PARAMETERS'
	defb	0

	psect	text
 	call	is_vsh ; is de vshell opgezet ?
        call	nz, setvsh ; zo niet neerzetten,
        ; vshell is nu opgezet en iy is exitcode - 9

        ld	c,60h ; _FORK
        call	bDos
	jr	z,2f
1:
	ld 	(_errno),a
	ld	hl,-1
	jp	cret
2:
	ld	a,b
	ld	(ppidp),a


; The handles in fds[] are used for the stdin/stdout/stderr of the program we
;  start, works only if these handles are inheritable!
	ld	(ixp),ix
	ld	l,(ix+10)
	ld	h,(ix+11) ; fds
	xor	a
4:
	cp	(hl)
	ld	b,a
	jr	z,5f
	push	af
	ld	c,45h ; _CLOSE
	push	hl
	call	bDos
	pop	hl
	ld	b,(hl)
	ld	c,47h ; _DUP
	push	hl
	call	bDos
	pop	hl
	ld	e,a
	pop	af
5:		
        cp	b
	jr	nz,reopen_err
	inc	hl
	inc	a
	cp	3
	jr	c,4b

 	ld	c,43h ; _OPEN
 	ld	de,mypages * 4000h 
        ld	a,1 ; no write
        call	bDos
        jr	nz,open_err ; unlikely, file is known to exist
	; handle in b
        ld	c,mypages   ; # pages
	ld	de,vramcode
        ld	hl,(6)
        inc	hl
        inc	hl
        inc	hl
        call	indir ; do_push, only returns on faillure
;        jr	c, novram

novram:
 	ld	e,0DEh
reopen_err:
	ld	a,e
open_err:
 	ld	(_errno),a
	ld	bc,(ppidp-1) ; pid in b
	ld	c,061h ; _JOIN
	call	bDos ; DOS2
	ld	ix,(ixp)
	ld	hl,-1
        jp	cret

psect	text
; kijkt of de exitcode in het geheugen staat
; UIT:	Z vlag aan -> de code staat er, iy = exitcode-9
;	Z vlag uit -> de code staat er niet, iy ongewijzigd
is_vsh:
 	ld	hl,(6)
        ld	l, .low. (magic-exitcode+9) ; jp nn en jr do_push overslaan
        ld	de,magic
        ld	b,3
2:
        ld	a,(de)
        cp	(hl)
        ret	nz ; vshell niet aanwezig
        inc	hl
        inc	de
        djnz	2b
	ld	l,0
	push	hl
        pop	iy
        ret

; zet de code voor een shell aan de top van het TPA
; UIT: iy - adres kopie exitcode - 9
setvsh: 
; look for VSHTOP in environment
; if it exists, stop testing for videomemory pages at its value 
psect	data
vshtopn:
	defm	'VSHTOP'
	defb	0
psect	bss
vshtopv:
	defs	4

psect	text
	ld	hl, vshtopn
	ld	de, vshtopv
	ld	bc,046Bh ; 4 bytes buffer space, _GENV
	call	bDos ; DOS2
	jr	nz,3f ; too long
	push	de
global	_atoi
	call	_atoi
	pop	de
	ld	a,h
	or	a
	jr	nz,3f	; way too high
	ld	a,l
	cp	12
	jr	nc,3f	; too high
	cp	2
	jr	c,3f	; too small
	add	a,070q
	ld	(topp),a
3:
        ld	hl,6
	ld	a,(0fcc1h) ; exptbl : slotadres bios
        call	0Ch ; RDSLT
        ld	(vdprd_),a ; poortnr lezen Vram/Xram
        inc	hl
	ld	a,(0fcc1h) ; exptbl : slotadres bios
        call	0Ch ; RDSLT
        inc	a
        ld	(vdpcmd_),a ; poortnr vdp commando's

; turn off interrupts while working with jump addresses and the vdp
	di
 	ld	a,(0007h)
        ld	h,a
        dec	a
        ld	(0007h),a ; msB nieuwe top van TPA
        ld	d,a
        ld	bc,09h
        ld	l,b
        ld	e,b
	push	de
	pop	iy
        ldir	; negen bytes vanaf oude top van TPA
        ld	hl,exitcode
        ld	bc,lenexit 
        ldir	; eigen exitcode kopieren
	push	de
        ld	bc,setvdpoff-lenexit-exitcode
        ex	de,hl
        add	hl,bc ; positie kopie setvdpoff
        ex	de,hl ; naar de
        ld	hl,setoff0+1-setvdpoff
        add	hl,de
12:
        ld	c,(hl)
        ld	(hl),e
        inc	hl
        ld	b,(hl)
        ld	(hl),d
        add	hl,bc
        jr	nc, 12b

	pop	de        
        ld	bc,setvdpon-lenexit-exitcode
        ex	de,hl
        add	hl,bc ; positie kopie setvdpon
        ex	de,hl ; naar de
        ld	hl,seton0+1-setvdpon
        add	hl,de
13:
        ld	c,(hl)
        ld	(hl),e
        inc	hl
        ld	b,(hl)
        ld	(hl),d
        add	hl,bc
        jr	nc, 13b
        
        ld	hl,(0001); pointer naar BIOS-Jump vector
        inc	l ; skip c3h, l wordt 4
        ld	c,(hl)
        ld	(iy+old_wb),c
        ld	(hl), .low. (new_wb-exitcode+9); nieuwe pos. exitcode
        inc	l
        ld	b,(hl)
        ld	(iy+old_wb+1),b
        ld	(hl),a ; bevat nog steeds msB nieuwe top TPA

; tel het video geheugen
 	ld	a,071q
2:
        call	vramtest
        jr	nz, 1f
        inc	a
        cp	104q
topp	equ	$-1
        jr	nz,2b
1:
 	ld	(iy+top),a
        ld	(iy+shsp),a
        ld	de,0
        jp	setvdpon ; adresregisters vdp terugzetten, scherm aan

vramtest:
 	ld	d,a
        ld	e,0 ; lezen
        call	setvdpoff
        ld	c,(iy+vdprd)
        in	l,(c)
        ld	e,40h ; schrijven
        in	h,(c)
        call	setvdpoff
        dec	c
        ld	a,l
        cpl
        out	(c),a
        ld	a,h
        cpl
        out	(c),a
        ld	e,0 ; lezen
        call	setvdpoff
        ld	c,(iy+vdprd)
        in	a,(c)
        cpl
        sub	l
        ld	b,a
        in	a,(c)
        cpl
        sub	h
        or	b
        ld	e,40h ; schrijven
        call	setvdpoff
        dec	bc	; dec c verandert F, daarom dec bc
        out	(c),l
        ld	a,d
        out	(c),h
        ret

global __cpm_clean        
__cpm_clean:
 	call	is_vsh
        ret	nz ; geen vshell, dan niks weg te halen
        ld	a,(iy+top)
        cp	(iy+shsp)
        ret	nz ; iets op de stack, dan vshell laten staan
; nu hebben we dus iets op te ruimen...
 	di
 	ld	hl,0007h
        inc	(hl)
        ld	hl,(0001h)
        inc	l
        ld	a,(iy+old_wb)
        ld	(hl),a
        inc	l
        ld	a,(iy+old_wb+1)
        ld	(hl),a
        ei
        ret

; ===========================================================================
; ==== code die in het videogeheugen wordt opgeslagen en door de         ====
; ==== exitcode vlak onder de exitcode wordt in geladen en uitgevoerd    ====
; ===========================================================================

vramcode:

; herstel slotselectie en mapper registers
; ---
enaslt	equ	024h

	ld	a, 08Bh ; slotadres van mapper, wordt overschreven
ramsltp	equ	$-1
	push	af
	ld	h, 0
	call	enaslt
	ld	a,3 	; segment in page 0, wordt overschreven
map0p	equ	$-1
	call	03050h	; adres van put_p0 komt hier
putp0p	equ	$-2 
	pop	af
	 
	push	af
	ld	h, 40h
	call	enaslt
	ld	a,2 	; segment in page 1, wordt overschreven
map1p	equ	$-1
	call	03150h	; adres van put_p1 komt hier
putp1p	equ	$-2 
	pop	af
	 
	ld	h, 80h
	call	enaslt
	ld	a,1 	; segment in page 2, wordt overschreven
map2p	equ	$-1
	call	03250h	; adres van put_p2 komt hier
putp2p	equ	$-2

; lees de inhoud van de eigen pagina's terug
; ---
	ld	a,mypages
        ld	d,(iy+shsp)
	ld	e,3Fh
        ld	hl,100h
        jr	rdblk
pglus:
	call	setvdpoff ; e is 0 again -> read
			  ;  page 0 has been restored by the time we get here
        ld	e,40h
rdblk:
        ld	c,(iy+vdprd)
 	ld	b,0
 	inir
 	dec	e
        jr	nz,rdblk
 	inc	d
        dec	a
        jr	nz,pglus
        ld	(iy+shsp),d
        ld	d,e ; de wordt 0
	call	setvdpon ; vdp adresregisters terugzetten, scherm aan
	ei

	ld	b,1 ; ppid, wordt overschreven
ppidp	equ	$-1
	ld	ix, 05849h ; ix, wordt overschreven
ixp	equ	$-2

; -- this part can also be executed while still in page 0
spwnret:
        ld	c,61h ; _JOIN
        ld	a,b ; ppid
        or	a
        call	nz,5 ; DOS2
        ld	a,b ; primary error code
        or	a
        ld	l,b
        jr	nz,errset
        ld	a,(80h) ; try value at, 80h if none given via DOS
	cp	04Fh ; old value of (80h), will be filled in by _spawn()
lenp	equ	$-1
	jr	z,errset ; probably not set by program
	ld	l,a	
errset:
        xor	a
        ld	h,a
 	jp	cret

lenvram	equ	$-vramcode

; ===========================================================================
; ==== code die naar de bovenkant van het TPA in page 3 gekopieerd wordt ====
; ===========================================================================

psect	text
exitcode: ;  --> xx09h
 	jr	do_push
magic:  ; ..b
 	defm	'vsh'
jump:
	defb	0C3h ; jp
data:	; ..f
 	defw	0 ; overschreven door hoofdprog.
 	defb	0104q, 0104q
vdprd_:
	defb	98h
vdpcmd_:
	defb	99h


old_wb	equ	data-exitcode+9
shsp	equ	old_wb+2
top	equ	shsp+1
vdprd	equ	shsp+2
vdpcmd	equ	shsp+3
        
new_wb: ; --> xx15h
 	ld	hl,(0006h)
	ld	l,0
	push	hl
	pop	iy
        ld	d,(iy+shsp)
        ld	a,(iy+top)
        cp	d
        jr	nz, do_pop
        ld	l, .low. (jump-exitcode+9)
        jp	(hl)
do_pop:
	dec	h
        ld	sp,hl
        di
        ld	e,0 ; lezen in vram, pagenr. in d
setoff0:call	setoff2-setoff0-1 ; offset naar volgende plek waar het adres
				  ; 	van de kopie van setvdpoff moet worden
				  ;	ingevuld
        ld	c,(iy+vdprd)
	ld	b,0
	inir
	dec	h
	jp	(hl)

; bewaart het werkgeheugen in video geheugen en laadt en runt dan 
; het programma waarvan de filehandle in D staat.
; Return via new_wb. De stackpointer wordt niet bewaard omdat
; deze toch al wordt hersteld bij een jp cret
; 
; IN:
;	(sp) -	return adres indien vram vol
;	iy   -	adres exitcode -9
;	de   -	adres van terugkeercode, wordt door new_wb geladen op iy-100h
;		en daar uitgevoerd
;	b    -	filehandle voor _READ
;	c    -	aantal pages om op te slaan
do_push:
 	ld	a,(iy+shsp)
        sub	c
        cp	071q
        ret	c ; not enough space
        
mem_ok:
 	di
        ld	sp,iy ; switch to new stack
        push	bc ; handle, #pages
        ld	(iy+shsp),a
	ex	de,hl
        ld	d,a
	ld	a,c     ; #pages
        ld	e,40h	; schrijven
setoff2:call	setoff3-setoff2-1 ; nog een stap verder ...
        dec	c	; vdpwr
	ld	b,0
	otir	; store 100h bytes with code to restore calling program in vram

        ld	hl,100h ; TPA starts at 100h
        jr	blkdone
wpglus:
 	inc	d
        ld	e,40h ; schrijven, 40h maal 100h bytes
setoff3:call	-1 ; de laatste stap
        dec	c
wrblk:
 	ld	b,0
 	otir
blkdone:
 	dec	e
        jr	nz,wrblk
        dec	a
        jr	nz,wpglus
        ld	d,e ; e=0; adresregisters goed te zetten
seton0:	call	-1  ; add hl,-1 geeft carry -> stoppen met invullen
        ei
        pop	bc ; file handle in b
	push	de ; 0: return adres voor het te laden programma 
        ld	de,100h
        push	de ; save load address as start adress
        push	bc
        ld	hl,-500h ; keep at least 1KiB space for stack
        add	hl,sp
        ld	c,48h ; _READ
        call	bDos
        jp	nz,0 ; als het fout gaat, return via new_wb
        pop	bc
        ld	c,45h ; _CLOSE
        call	bDos
	ld	a,1
	ld	(37h),a ; set load flag; PROGRAM and PARAMETERS set in _spawn
	ret	; to 100h

; stelt vdp in voor lezen of schrijven
; IN 	- d= 0xyz octaal 
; 		x=1: external ram, x=0: video ram
;		z = bit 17..15 van adres (bit 14..0 zijn nul)
;	- e=0h: lezen, e=40h: schrijven, 
;	- iy = exitcode-9
; OUT	- c=vdp write-adr+1
; WIJZIGT: bc, af'
setvdpon:
	ex	af,af'
	ld	a,(RG1SAV)
	defb	01h ; ld bc, 
setvdpoff:
 	ex	af,af' ;   0x..08
	xor	a      ;  +0xaf..
        ld	c,(iy+vdpcmd)
	out	(c),a
	ld	a,81h
	out	(c),a ; schrijf reg 1: scherm aan/uit
	rrca	; a wordt 0300q
        and	d ; a wordt 0100q (ext. ram) of 0 (norm. vram)
        out	(c),a
        ld	a,80h+45
        out	(c),a ; reg. 45 schrijven
        ld	a,7
        and	d
        out	(c),a
        ld	a,80h+14
        out	(c),a ; reg. 14 schrijven
        xor	a
        out	(c),a
        out	(c),e ; stel adres en mode in
        ex	af,af'
        ret
 	
lenexit equ	$-exitcode
; ===== eind van gekopieerde code

RG1SAV equ      0F3E0h

	end	start

