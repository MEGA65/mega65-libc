.global _shres_trap
.global _shres_regs

.section .text

; _shres_trap()
; Sends a 32-bit argument in _shres_regs[0..3] to $D645,
; then stores 32-bit return result back into _shres_regs[0..3]
; and processor flags into _shres_regs[4].

_shres_trap:	

phy
phz
	
    lda _shres_regs+0
    ldx _shres_regs+1
    ldy _shres_regs+2
    ldz _shres_regs+3
	
    sta $D645           ; Store to the MEGA65 SYSPART trap address
    nop                 ; Delay/stabilize (preserved from original)
	

	; Store result back into _shres_regs
    sta _shres_regs+0
    stx _shres_regs+1
    sty _shres_regs+2
    stz _shres_regs+3
	
    php
    pla
    sta _shres_regs+4   ; Save status register into 5th byte
    ldx #0
	txa

	plz
	ply
	
    rts

; Register block: 4 bytes for argument/result, 1 byte for flags

	.section .bss
_shres_regs:
	.space 5


	.section .text
