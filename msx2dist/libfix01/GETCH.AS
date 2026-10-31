; Hi-Tech C standard library
;
; Accelerated MSXDOS versions of the following console functions
; (defined in conio.h):
;
; char	   getch(void);    /* get character */
; char	   getche(void);   /* get/echo character */
; void	   putch(int);	   /* output character to console;/
; void	   ungetch(int);   /* unget character;/
; int	   kbhit(void);    /* true if keyboard hit;/
;
; added new function:
;
; void	   cursor(int); 	       [ 1 = on,  0 = off ]
;
; Use this file to update the Hitech-C standard library libc.lib.
;
; PGN 10/93
; Last update 12/93


;void cursor(char onoff)

	psect	text
	global	_cursor
_cursor:
	ld	a,l
	ld	(0fca9h),a	; 0=off else on
	ld	(0fcaah),a	; line style cursor if on
	ret

;char getch(void)

	psect	text
	global	_getch
_getch:
	ld	hl,(pushbk)
	ld	a,h
	or	l		; if pushed back
	jr	z,1f		; none
	ld	de,0
	ld	(pushbk),de	; else delete pushback
	ret			; return with hl
1:	push	ix
	push	iy
	ld	ix,09fh
	ld	iy,(0fcc0h)
	call	01ch		; chget
	ld	l,a
	ld	h,0
	pop	iy
	pop	ix
	ret


;char getche(void)

	psect	text
	global	_getche
_getche:
	ld	hl,(pushbk)
	ld	a,h
	or	l		; if pushed back
	jr	z,2f		; none
	ld	de,0
	ld	(pushbk),de	; else delete pushback
	ret			; return with hl
2:	call	1b		; get character
	push	hl
	call	_putch
	pop	hl
	ret

;void ungetch(char)

	psect	text
	global	_ungetch
_ungetch:
	ld	(pushbk),hl
	ret
	psect	data
pushbk:
	defw	0

;void putch(char)

	psect	text
	global	_putch
_putch:
	ld	a,l
	cp	10		; if newline
	jr	nz,3f
	ld	hl,13
	call	3f
	ld	hl,10
3:	push	ix
	push	iy
	ld	a,l
	ld	ix,0a2h
	ld	iy,(0fcc0h)
	call	01ch
	pop	iy
	pop	ix
	ret

;int kbhit()

	psect	text
	global	_kbhit
_kbhit:
	ld	hl,(0f3fah)	; getpnt
	ld	a,(0f3f8h)	; putpnt
	cp	l
	ld	hl,1
	ret	nz		; if NZ then keypressed
	ld	hl,0
	ret

; end of file
