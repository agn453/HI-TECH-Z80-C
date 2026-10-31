; These routines were previously in inout.as, I've put them in a 
; separate file because they can be easiliy rewritten, so they need not
; take up space in a .com file - Arnold M
 
	global _peek, _poke
	psect	text

; void poke(word address, byte value)

_poke:
        pop     hl              ;return address
        pop     de              ;address
        pop     bc              ;value
        push    bc
        push    de
	ld	a,c
        ld      (de),a
	jp	(hl)

; byte peek(word address)

_peek:
        pop     de              ;return address
        pop     hl              ;peek address
        push    hl
        push    de
        ld      l,(hl)
        ld      h,0
        ret


