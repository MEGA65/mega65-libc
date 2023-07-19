.global lpeek, lpoke
.section .zeropage, "aw", @nobits
tmp:
        .ds.b 4

.section code,"a"
lpoke:
        ; 32-bit address (a, x, rc2, rc3)
        sta tmp
        stx tmp + 1
        lda __rc2
        sta tmp + 2
        lda __rc3
        sta tmp + 3
        ; 8-bit value (rc4)
        lda __rc4
        ldz #0
        nop
        sta (tmp), z
        ldz #0
        rts
lpeek:
        ; 32-bit address (a, x, rc2, rc3)
        sta tmp
        stx tmp + 1
        lda __rc2
        sta tmp + 2
        lda __rc3
        sta tmp + 3
        ldz #0
        ldx #0
        nop
        lda (tmp), z
        ldz #0
        rts