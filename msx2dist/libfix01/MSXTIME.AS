; time_t msxtime();
; used by time(), timerset() and timeup() functions
; Public domain by Pierre Gielen.

	psect text
	global _msxtime

_msxtime:
	push	ix
	push	iy
	ld	c,02ah		; get date
	call	5

	ld	bc,-1970	; unix offset
	add	hl,bc		; hl=jaren sinds 1970, d=maanden, e=dag
	push	de		; bewaar e=dag, d=maand
	ld	a,l
	add	a,70		; *** LEAPYEAR BUGFIX ***
	push	af		; bewaar jaar
	ld	c,d		; c=maand
	ld	b,l		; b jaren sinds 1970
	ld	hl,0		; vanaf 0
	ld	a,1		; modulo 4 teller
	ld	de,365		; dagen per jaar
1:	add	hl,de		; tel 1 jaar in dagen er bij op
	inc	a		; verhoog modulo 4 teller
	and	3		; elke 4 jaar
	jr	nz,2f		; nee
	inc	hl		; ja, 1 dag extra voor schrikkeljaar
2:	djnz	1b		; en dat b keer achter elkaar

; hl bevat nu het aantal dagen in de jaren sinds 1970

	pop	af		; het huidige jaar
	and	3		; is dit een schrikkeljaar?
	jr	nz,3f		; nee
	ld	a,c		; ja, dan pak maand
	cp	3		; als februari voorbij is
	jr	c,3f		; nog niet
	inc	hl		; wel, dan 1 extra voor 29 februari
;
3:	ld	b,c		; maand in b
	ld	de,17f		; tabel met aantal dagen per maand
	jr	5f		; bereken dagen
;
4:	call	7f		; tel aantal dagen in deze maand bij totaal
	inc	de		; verhoog maandentabel-pointer
5:	djnz	4b		; volgende (voor b maanden)

	pop	de		; de dag staat nog in e
	ld	a,e
	dec	a		; offset 0 (niet 1)
	call	6f		; tel op bij totaal
	jr	8f		; klaar

7:	ld	a,(de)		; add day of current month
6:	add	a,l
	ld	l,a
	ret	nc
	inc	h
	ret

8:	call	9f
	ld	(95f),hl      ; zet totaal in long1 buffer
	ld	hl,0
	ld	(95f+2),hl
	inc	hl
	ld	(96f+2),hl
	ld	hl,05180h
	ld	(96f),hl      ; zet 15180h (=seconden per dag) in LONG2
	call	10f	      ; long1 = long1 * long2
	call	11f	      ; long3 = long1

	ld	c,02ch		; get time
	call	5

	push	de		; bewaar d=seconden
	push	hl		; bewaar h=uren, l=minuten

	call	9f
	ld	l,h		; hl=uren
	ld	h,0
	ld	(95f),hl
	ld	hl,3600
	ld	(96f),hl
	call	10f
	ld	hl,(95f)
	ld	(96f),hl
	ld	hl,(95f+2)
	ld	(96f+2),hl    ; verplaats long 1 naar long2
	call	12f	      ; haal jaren in seconden
	call	13f	      ; long1=long1+long2

	ld	hl,0		; offset 0
	pop	de		; haal e=minuten (was l)
	ld	a,e
	or	a		; 0?
	jr	z,15f		; dan hebben we dit niet nodig
	ld	d,0
	ld	b,60		; seconden per minuut
16:
	add	hl,de
	djnz	16b		; tel minuten in seconden op in hl

15:	ld	(96f),hl	; in long2 (long1 nog ok)
	ld	hl,0
	ld	(96f+2),hl
	call	13f		; tel minuten op

	pop	hl		; haal h=seconden (was d)
	ld	l,h		; in l
	ld	h,0
	ld	(96f),hl
	ld	hl,0
	ld	(96f+2),hl
	call	13f		; tel seconden op

	ld	de,(95f)
	ld	hl,(95f+2)    ; resultaat nu in hlde, zoals Hitech wenst
	pop	iy
	pop	ix
	ret


	psect data

; tabel met aantal dagen per maand (geen schrikkeljaar)

17:	defb	31,28,31,30,31,30,31,31,30,31,30,31

	psect text

11:	push	hl
	ld	hl,(95f)
	ld	(97f),hl
	ld	hl,(95f+2)
	ld	(97f+2),hl
	pop	hl
	ret

12:	push	hl
	ld	hl,(97f)
	ld	(95f),hl
	ld	hl,(97f+2)
	ld	(95f+2),hl
	pop	hl
	ret

9:	push	hl
	ld	hl,0
	ld	(95f),hl
	ld	(95f+2),hl
	ld	(96f),hl
	ld	(96f+2),hl
	pop	hl
	ret

10:	ld	hl,95f
	ld	de,96f
	ld	a,4
	ld	c,4
	ld	b,0
	add	hl,bc
	ex	de,hl
	ld	(93f),hl
	ld	hl,94f
	add	hl,bc
	ld	(92f),hl
	ld	l,c
	ld	h,b
	add	hl,hl
	add	hl,hl
	add	hl,hl
	inc	hl
	ld	(91f),hl

	ld	b,c
	ld	hl,94f

18:	ld	(hl),0
	inc	hl
	djnz	18b

	and	a
19:	ld	b,c
	ld	hl,(92f)

20:	dec	hl
	rr	(hl)
	djnz	20b

	ld	l,e
	ld	h,d
	ld	b,c

21:	dec	hl
	rr	(hl)
	djnz	21b

	jp	nc,22f

	push	de
	ld	de,(93f)
	ld	hl,94f
	ld	b,c
	and	a

23:	ld	a,(de)
	adc	a,(hl)
	ld	(hl),a
	inc	de
	inc	hl
	djnz	23b
	pop	de

22:	ld	a,(91f)
	dec	a
	ld	(91f),a
	jp	nz,19b
	push	af
	ld	a,(91f+1)
	and	a
	jp	z,24f
	dec	a
	ld	(91f+1),a
	pop	af
	jp	19b

24:	pop	af
	ret

13:	ld	hl,95f
	ld	de,96f
	ld	b,4		; lengte
14:	ld	a,(de)
	adc	a,(hl)
	ld	(hl),a
	inc	hl
	inc	de
	djnz	14b
	ret

	psect data

91:	defs	2		; teller
92:	defs	2
93:	defs	2
94:	defs	8
95:	defs	4		; buffers voor longints
96:	defs	4
97:	defs	4

