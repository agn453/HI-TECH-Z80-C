;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Arctangent of a radian result.
;	The C stack is left unchanged.

	global	_atan, cunary

	psect	text

_atan:
	ld	a,07h
	jp	cunary
