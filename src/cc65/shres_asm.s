
	.setcpu "65C02"
	.export _shres_trap
	
.SEGMENT "CODE"

	.p4510
	
	;; closedir takes file descriptor as argument (appears in A)
_shres_trap:
	NEG  			; Prefix instructions to make LDA -> LDQ
	NEG
	LDA _shres_regs
	STA $D645
	NOP
	NEG			; Prefix instructions to make STA -> STQ
	NEG
	STA _shres_regs
	PHP
	PLA
	STA _shres_regs+4
	LDX #$00
	TAX
	RTS

_shres_regs:	
	.dword 0   		; regs
	.byte 0 		; processor flags
