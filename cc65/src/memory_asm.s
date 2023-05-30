        .export _qpeek

        .importzp sreg,tmp1,tmp2,tmp3,tmp4

        .p4510

.proc	_qpeek: near

.segment	"CODE"
        sta tmp1
        stx tmp2
        lda sreg
        sta tmp3
        lda sreg+1
        sta tmp4
        ldz #$00
        ldx #$00
        nop
        lda (tmp1),z
        rts
.endproc
