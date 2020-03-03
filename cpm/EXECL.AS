;	EXECV(), EXECL() - hand crafted from the C

;	#include	<cpm.h>
;	
;	#define	setuid(x)	bdos(CPMSUID, x)
;	#define	getuid()	bdos(CPMSUID, 0xFF)
;	#define	TPA		0x100
;	#define	DBUF		0x80
;	#define	DFCB		0x5C
;	
;	static void
;	doexec(fc, luid)
;	register struct fcb *	fc;
;	uchar			luid;
;	{
;		char *	dma;
;	
;		dma = (char *)TPA;
;		do {
;			bdos(CPMSDMA, dma);
;			dma += SECSIZE;
;		} while(bdos(CPMREAD, fc) == 0);
;		setuid(luid);
;		bdos(CPMSDMA, DBUF);
;		(*(void (*)())TPA)();
;	}
;	
;	
;	execl(name, arg1)
;	char *	name, arg1;
;	{
;		execv(name, &arg1);
;	}
;	
;	execv(name, arg)
;	char *	name, ** arg;
;	{
;		struct fcb	fc;
;		uchar		luid;
;		short		i, j;
;		register char *	cp;
;		char		progbuf[128];	/* storage for the code */
;	
;		for(i = 1, j = 0 ; arg[i] ; i++)
;			j += strlen(arg[i])+1;
;		if(j >= 126)
;			return -1;		/* arg list too big */
;		if(i > 1)
;			setfcb(DFCB, arg[1]);
;		else
;			setfcb(DFCB, "");
;		cp = (char *)DBUF;
;		*cp++ = j;
;		*cp = 0;
;		for(i = 0 ; arg[i] ; i++) {
;			strcat(cp, " ");
;			strcat(cp, arg[i]);
;		}
;		setfcb(&fc, name);
;		if(fc.ft[0] != ' ')
;			return -1;
;		strncpy(fc.ft, "COM", 3);
;		luid = getuid();
;		setuid(fc.uid);
;		if(bdos(CPMOPN, &fc) == -1) {
;			setuid(luid);
;			return -1;
;		}
;		bmove(doexec, progbuf, sizeof progbuf);
;		(*(void (*)())progbuf)(&fc, luid);
;	}
*Title HI-TECH C: EXECV.C

entry	equ	5		; CP/M bdos call entry point
global	indir

psect	text
_doexec:
push	iy
push	ix
ld	ix,0
add	ix,sp
push	bc
ld	c,(ix+6)
ld	b,(ix+7)
push	bc
pop	iy
ld	(ix+-2),0
ld	(ix+-1),1
l5:
ld	e,(ix+-2)
ld	d,(ix+-1)
ld	c,26
push	ix
push	iy
call	entry
pop	iy
pop	ix
ld	de,128
ld	l,(ix+-2)
ld	h,(ix+-1)
add	hl,de
ld	(ix+-2),l
ld	(ix+-1),h
push	iy
pop	de
ld	c,20
push	ix
push	iy
call	entry
pop	iy
pop	ix
or	a
jr	z,l5
ld	e,(ix+8)
ld	c,32
call	entry
ld	de,128
ld	c,26		;SETDMA
call	entry
call	256		;go execute at TPA base
jp	0		;if it returned
global	_execl
global	_execv
_execl:
push	iy
push	ix
ld	ix,0
add	ix,sp
push	ix
pop	de
ld	hl,8
add	hl,de
push	hl
ld	l,(ix+6)
ld	h,(ix+7)
push	hl
call	_execv
ld	sp,ix
pop	ix
pop	iy
ret
global	_strlen
global	_setfcb
global	_strcat
global	_strncpy
global	_bmove
_execv:
push	iy
push	ix
ld	ix,0
add	ix,sp
ld	hl,-175
add	hl,sp
ld	sp,hl
ld	(ix+-45),1
ld	(ix+-44),0
ld	(ix+-47),0
ld	(ix+-46),0
jp	l11
l8:
ld	e,(ix+8)
ld	d,(ix+9)
ld	l,(ix+-45)
ld	h,(ix+-44)
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
call	_strlen
pop	bc
ex	de,hl
inc	de
ld	l,(ix+-47)
ld	h,(ix+-46)
add	hl,de
ld	(ix+-47),l
ld	(ix+-46),h
ld	l,(ix+-45)
ld	h,(ix+-44)
inc	hl
ld	(ix+-45),l
ld	(ix+-44),h
l11:
ld	e,(ix+8)
ld	d,(ix+9)
ld	l,(ix+-45)
ld	h,(ix+-44)
add	hl,hl
add	hl,de
ld	a,(hl)
inc	hl
or	(hl)
jp	nz,l8
ld	de,126
ld	l,(ix+-47)
ld	h,(ix+-46)
or	a
sbc	hl,de
jp	m,l12
L2:
ld	hl,-1
ld	sp,ix
pop	ix
pop	iy
ret
l12:
ld	l,(ix+-45)
ld	h,(ix+-44)
ld	de,1
or	a
sbc	hl,de
jp	m,l13
ld	l,(ix+8)
ld	h,(ix+9)
inc	hl
inc	hl
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
ld	hl,92
push	hl
call	_setfcb
pop	bc
pop	bc
jp	l14
l13:
ld	hl,19f
push	hl
ld	hl,92
push	hl
call	_setfcb
pop	bc
pop	bc
l14:
ld	iy,128
ld	a,(ix+-47)
ld	(iy+0),a
inc	iy
ld	(iy+0),0
ld	(ix+-45),0
ld	(ix+-44),0
jp	l18
l15:
ld	hl,39f
push	hl
push	iy
call	_strcat
pop	bc
pop	bc
ld	e,(ix+8)
ld	d,(ix+9)
ld	l,(ix+-45)
ld	h,(ix+-44)
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
push	iy
call	_strcat
pop	bc
pop	bc
ld	l,(ix+-45)
ld	h,(ix+-44)
inc	hl
ld	(ix+-45),l
ld	(ix+-44),h
l18:
ld	e,(ix+8)
ld	d,(ix+9)
ld	l,(ix+-45)
ld	h,(ix+-44)
add	hl,hl
add	hl,de
ld	a,(hl)
inc	hl
or	(hl)
jp	nz,l15
ld	l,(ix+6)
ld	h,(ix+7)
push	hl
push	ix
pop	de
ld	hl,-42
add	hl,de
push	hl
call	_setfcb
pop	bc
pop	bc
push	ix
pop	de
ld	hl,-33
add	hl,de
ld	a,(hl)
cp	32
jp	nz,L2
ld	hl,3
push	hl
ld	hl,29f
push	hl
push	ix
pop	de
ld	hl,-33
add	hl,de
push	hl
call	_strncpy
pop	bc
pop	bc
pop	bc
ld	e,255
ld	c,32
push	ix
push	iy
call	entry
pop	iy
pop	ix
ld	(ix+-43),a
ld	e,(ix+-1)	;get fc.uid
ld	c,32		; CPMSUID
push	ix
push	iy
call	entry
pop	iy
pop	ix
push	ix
pop	de
ld	hl,-42
add	hl,de
ex	de,hl
ld	c,15
push	ix
push	iy
call	entry
pop	iy
pop	ix
cp	255
jp	nz,l20
ld	e,(ix+-43)
ld	c,32
push	ix
push	iy
call	entry
pop	iy
pop	ix
jp	L2
l20:
ld	hl,128
push	hl
push	ix
pop	de
ld	hl,-175
add	hl,de
push	hl
ld	hl,_doexec
push	hl
call	_bmove
pop	bc
pop	bc
pop	bc
ld	l,(ix+-43)
ld	h,0
push	hl
push	ix
pop	de
ld	hl,-42
add	hl,de
push	hl
push	ix
pop	de
ld	hl,-175
add	hl,de
call	indir
ld	sp,ix
pop	ix
pop	iy
ret
psect	data
19:
defb	0
29:
defb	67,79,77,0
39:
defb	32,0
