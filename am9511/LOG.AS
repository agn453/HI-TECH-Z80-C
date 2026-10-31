;
;  Copyright (c) 2026 Phillip Stevens
;
;  This Source Code Form is subject to the terms of the MIT License.
;  If a copy of the license was not distributed with this file,
;  You can obtain one at https://opensource.org/licenses/MIT.
;
;  feilipu, October 2026
;
;	Natural log and common log.
;	A non-positive argument returns 0.
;	log10 is the chip LOG opcode, not a scale of ln.

	global	_log, _log10, cunary

	psect	text

_log:
	ld	a,09h
	jp	cunary

_log10:
	ld	a,08h
	jp	cunary
