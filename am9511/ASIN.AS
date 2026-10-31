;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Arcsine. A magnitude above 1 returns 0.
;	The C stack is left unchanged.

	global	_asin, cunary

	psect	text

_asin:
	ld	a,05h
	jp	cunary
