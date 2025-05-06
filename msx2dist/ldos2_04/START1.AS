	psect	text
	global	__getargs, startup, __argc_, bDos

startup:
; ensure the program is running under msx-dos2
	ld c, 06Fh ; _DOSVER (MSX-DOS2)
	call bDos
	or a
	jr nz, nodos2
	ld a,b
	cp 2
	jr c, nodos2

;close AUX and PRN,which are not supposed to be open 
;    when the program starts, should be harmless under DOS1
        ld bc, 0345h ; _CLOSE, handle 3:AUX
        call bDos
        ld bc, 0445h ; _CLOSE, handle 4:PRN
        call bDos
	jp	__getargs

nodos2:
	ld de, errmsg
	ld c, 09h ; _STROUT
	call bDos
	ld c,00h ; _TERM0
	call bDos

	psect	data
errmsg:
	defm	'*** wrong version of msx-dos.'
	defb	13,10,'$'

	psect	bss
__argc_:
	defs	2
