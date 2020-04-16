; closedir.as

; int closedir (DIR * dir)
; {
;   free(dir);
;   return 0;
; }
 
global _closedir
global _free
psect text

_closedir:
pop hl
pop de ; dir
push de
push hl
push de
call _free
pop de
ld hl,0
ret
