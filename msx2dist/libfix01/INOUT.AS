	global _in, _out
	global _di, _ei

	psect	text

; byte in(byte io-address)

_in:
	ld	c,l		;port address
	in	l,(c)		;read port
	ld	h,0		;zero extend it
	ret

; byte out(byte io-address,byte value)

_out:
	pop	hl		;return address
	pop	bc		;port address
	pop	de		;data
	push	de
	push	bc
	push	hl
	out	(c),e		;output the data
	ld	l,c		;return value in hl also
	ld	h,0
	ret

; void di();
; void ei();

_di:	ld	a,(0f3e0h)
        and     0dfh
        jr      1f

_ei:	ld	a,(0f3e0h)
        or      20h

1:      ld      b,a
        ld      c,1
        push    ix
        push    iy
        ld      ix,047h ; routine WRTVDP
        ld      iy,(0fcc0h) ; in main bios rom
        call    01ch
        pop     iy
        pop     ix
        xor     a
        ld      (0fca9h),a
	ret


	end
