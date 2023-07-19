.global lpeek
.section .zeropage, "a", @nobits
tmp:
        .ds.b 4
tmp_sp:
        .ds.b 1

.section code,"a"
; https://github.com/cc65/cc65/blob/master/libsrc/runtime/addysp.s
addysp:
        pha                     ; Save A
        clc
        tya                     ; Get the value
        adc     tmp_sp              ; Add low byte
        sta     tmp_sp              ; Put it back
        bcc     L1              ; If no carry, we're done
        inc     tmp_sp + 1          ; Inc high byte
L1:     pla                     ; Restore A
        ldz     #$00
        rts
incsp4:
        ldy     #4
        jmp     addysp
lpoke:
        pha
        ldz #$00
        neg
        neg
        neg
        sta tmp
        ldz #$00
        pla
        nop
        sta (tmp),z
        jmp incsp4

lpeek:
        sta tmp
        stx tmp + 1
        lda __rc2
        sta tmp + 2
        lda __rc3
        sta tmp + 3
        ldz #$00
        ldx #$00
        nop
        lda (tmp), z
        ldx #$00
        rts