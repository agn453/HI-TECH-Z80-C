;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Sine of a radian argument.
;	The C stack is left unchanged.

	global	_sin, cunary

	psect	text

_sin:
	ld	a,02h
	jp	cunary
