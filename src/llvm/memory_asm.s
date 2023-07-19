.global lpeek
.section .zeropage, "a", @nobits
tmp:
        .ds.b 4

.section code,"a"
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
        rts