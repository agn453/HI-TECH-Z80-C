; rewnddir.as

; void rewinddir (DIR * dir)
; {
;    dir->dir_file.d_valid = 0;
; }

global _rewinddir
psect text

_rewinddir:
pop hl
pop de ; dir
push de
push hl
ld hl, 64
add hl,de
ld (hl),0
ret
