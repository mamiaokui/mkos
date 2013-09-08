[BITS 32]

		GLOBAL	_start
        extern  MKOSMain

[SECTION .text]
_start:
        call MKOSMain
