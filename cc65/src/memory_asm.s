        .export _lpoke, _lpeek

        .p02

        .importzp sp,sreg,tmp1,tmp2,tmp3,tmp4
        .import incsp4
        sp4510 = sp

        .p4510
        

.segment	"CODE"

.proc	_lpoke: near
        pha
        ldz #$00
        neg
        neg
        lda (sp4510),z
        neg
        neg
        sta tmp1
        ldz #$00
        pla
        nop
        sta (tmp1),z
        jmp incsp4
.endproc

.proc	_lpeek: near
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
