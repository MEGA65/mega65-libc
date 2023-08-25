; To see how LLVM-MOS encodes this file, run:
;
;    llvm-mc -mcpu=mos45gs02 --show-encoding memory_asm.s
;
.global lpoke
.section .text.lpoke,"ax",@progbits
lpoke:
        ; copy 32-bit input address (a, x, rc2-rc3) to rc5-rc8
        sta __rc5
        stx __rc6
        lda __rc2
        sta __rc7
        lda __rc3
        sta __rc8
        ; 8-bit input value (rc4)
        lda __rc4
        ldz #0
        sta [__rc5], z
        rts

;.global lpeek
.section .text.llpeek,"ax",@progbits
llpeek:
        ; copy 32-bit input address (a, x, rc2-rc3) to rc4-rc7
        sta __rc4
        stx __rc5
        lda __rc2
        sta __rc6
        lda __rc3
        sta __rc7
        ldz #0
        lda [__rc4], z
        rts
