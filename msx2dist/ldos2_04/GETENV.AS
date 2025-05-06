;string.h: 4: typedef	int		ptrdiff_t;
;string.h: 5: typedef	unsigned	size_t;
;string.h: 18: extern char *	strcpy(char *, char *);
;string.h: 34: extern size_t	strlen(char *);
;stdlib.h: 27: extern void *	malloc(size_t);

;B:GETENV.C: 9: char *
;B:GETENV.C: 10: getenv(s)
;B:GETENV.C: 11: char *	s;
;B:GETENV.C: 12: {
psect	text
global	_getenv
_getenv:
global	ncsv, cret, indir, bDos
call	ncsv
defw	-255
;B:GETENV.C: 13:   register char *res;
;B:GETENV.C: 14:   char val[255];

  ld bc,0FF6Bh ; _GENV, buffersize=255
  push ix
  pop hl
  ld de,-255 
  add hl,de ; val
  ld (hl),0 ; make sure val is empty if *s invalid
  ex de,hl
  ld l,(ix+6) ; s
  ld h,(ix+7)
  call bDos
; out de==val
;B:GETENV.C: 29:   res= (strlen(val)==0) ? ((void *)0) : malloc(strlen(val)+1);
global	_strlen
global	_malloc
push	de
call	_strlen
pop bc
ld	a,l
or	h
jp	z,10f
push	ix
pop	de
ld	hl,-255
add	hl,de
push	hl
call	_strlen
pop	bc
inc	hl
push	hl
call	_malloc
pop	bc
jp	12f
10:
ld	hl,0
12:
ld	C,l
ld	B,h
;B:GETENV.C: 30:   return (res==((void *)0)) ? ((void *)0) : strcpy(res, val);
global	_strcpy
ld	a,l
or	h
jp	z,10f
push	ix
pop	de
ld	hl,-255
add	hl,de
push	hl
push	bc
call	_strcpy
pop	bc
jp	12f
10:
ld	hl,0
12:
;B:GETENV.C: 31: }
jp	cret

