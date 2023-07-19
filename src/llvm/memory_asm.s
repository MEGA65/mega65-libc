.global lpeek, lpoke
.section code,"a"
lpoke:
        ; copy 32-bit input address (a, x, rc2, rc3) to rc5
        sta __rc5
        stx __rc5 + 1
        lda __rc2
        sta __rc5 + 2
        lda __rc3
        sta __rc5 + 3
        ; 8-bit input value (rc4)
        lda __rc4
        ldz #0
        nop
        sta (__rc5), z
        ldz #0
        rts
lpeek:
        ; copy 32-bit input address (a, x, rc2, rc3) to rc4
        sta __rc4
        stx __rc4 + 1
        lda __rc2
        sta __rc4 + 2
        lda __rc3
        sta __rc4 + 3
        ldz #0
        ldx #0
        nop
        lda (__rc4), z
        ldz #0
        rts