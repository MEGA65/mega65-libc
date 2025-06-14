.global _shres_trap
.global _shres_regs

.section .text

; _shres_trap()
; Sends a 32-bit argument in _shres_regs[0..3] to $D645,
; then stores 32-bit return result back into _shres_regs[0..3]
; and processor flags into _shres_regs[4].

_shres_trap:
    ldq _shres_regs     ; Load 32-bit value from _shres_regs
    stq $D645           ; Store to the MEGA65 SYSPART trap address
    nop                 ; Delay/stabilize (preserved from original)

    stq _shres_regs     ; Store result back into _shres_regs

    php
    pla
    sta _shres_regs+4   ; Save status register into 5th byte
    ldx #0
    txa
    rts

.section .bss

; Register block: 4 bytes for argument/result, 1 byte for flags
_shres_regs:
    .zero 5
