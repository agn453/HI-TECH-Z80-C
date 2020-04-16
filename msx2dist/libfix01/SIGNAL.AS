; SIGNAL.C, manually modified for MSXDOS-2
;
;static void (*  where)();
;
;void (*signal(sig, action))()
;int	 sig;
;void (* action)(int);
;{
;	 void (* prev)();
;
;	 if(sig != SIGINT)
;		 return (void (*)())-1;
;	 prev = where;
;	 where = action;
;	 return prev;
;}

global	_signal
global	csv,cret
psect	text
_signal:
call csv
push hl
ld	de,1
ld	l,(ix+6)
ld	h,(ix+7)
or	a
sbc	hl,de
jr	z,1f
ld	hl,-1
jp	cret
1:
ld	hl,(_where)
ex	de,hl		; de = previous
ld	l,(ix+8)
ld	h,(ix+9)	; new _where
ld	(_where),hl
ex	de,hl		; return previous
jp	cret


;void _sigchk()
;{
;	 char	 c;
;
;	 if (where == SIG_IGN)
;	    return;
;	 if (bdos(CONSTAT) != 0) {
;	    c = bdos(CONIN);
;	    if (c != CPMRBT)
;	       return;
;	    if (where == SIG_DFL)
;	       exit(0);
;	    (*where)();
;	 }
;}

global	__sigchk,_exit,indir

__sigchk:
ld	a,(0fc9bh)	; INTFLG
cp	3		; if ctrl-stop pressed
jp	z,1f		; then ignore all bypasses

ld	hl,(0f3fah)	; getpnt
ld	a,(0f3f8h)	; putpnt
cp	l		; if not keypressed
ret	z		; then return

ld	de,1		; SIG_IGN
ld	hl,(_where)	; if where == SIG_IGN
or	a
sbc	hl,de
ret	z		; then return

push	ix
push	iy
ld	ix,09fh
ld	iy,(0fcc0h)
call	01ch		; _CHGET
pop	iy
pop	ix
cp	3		; if not CTRL-C
ret	nz		; then return

ld	hl,(_where)	; else look where to go
ld	a,l
or	h
jr	nz,2f		; not goto 0 ?

1:
ld	hl,0
push	hl
call	_exit		; default to exit(0)

2:
ld	hl,(_where)
call	indir
ret

psect	bss
_where:
defs	2

end


