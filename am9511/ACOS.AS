;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Arccosine. A magnitude above 1 returns 0.
;	The C stack is left unchanged.

	global	_acos, cunary

	psect	text

_acos:
	ld	a,06h
	jp	cunary
