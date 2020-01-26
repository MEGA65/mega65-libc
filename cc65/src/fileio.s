
	.setcpu "65C02"
	.export _closeall, _open, _close, _read512

	.include "zeropage.inc"
	
.SEGMENT "CODE"

	.p4510

cc65_args_read_ptr1_16:	
        ;; sp here is the ca65 sp ZP variable, not the stack pointer of a 4510
        .p02
        lda (sp),y
        sta ptr1
        iny
        lda (sp),y
        .p4510
        sta ptr1+1
	iny
	rts
        
cc65_args_read_tmp1_8:	
        ;; sp here is the ca65 sp ZP variable, not the stack pointer of a 4510
        .p02
        lda (sp),y
        sta tmp1
        iny
        .p4510
	rts
        

cc65_copy_ptr1_string_to_0100:	
        ;; Copy file name
	phy
        ldy #0
@NameCopyLoop:
        lda (ptr1),y
        sta $0100,y
        iny
        cmp #0
        bne @NameCopyLoop
	ply
	rts	

setname_0100:	
        ;;  Call dos_setname()
        ldy #>$0100
        ldx #<$0100
        lda #$2E                ; dos_setname Hypervisor trap
        STA $D640               ; Do hypervisor trap
        NOP                     ; Wasted instruction slot required following hyper trap instruction
        ;; XXX Check for error (carry would be clear)
	bcs setname_ok
	lda #$ff
	rts
setname_ok:
	RTS

	
	;; closeall
_closeall:
	TAX
	LDA #$22
	STA $D640
	NOP
	LDX #$00
	RTS

_read512:
        ;; Get pointer to FD and to buffer
	;; (Note we have to exract the args in reverse order)
        ldy #0
	jsr cc65_args_read_ptr1_16
	jsr cc65_args_read_tmp1_8

	;; Select current file
	;; XXX - Not currently implemented
	
	;; Read the next sector of data
	LDA #$1A
	STA $D640
	NOP
	LDX #$00

	;; Number of bytes read returned in X and Y
	;; Store these for returning
	stx tmp2
	sty tmp3

	;; Copy the full 512 bytes from the sector buffer at $FFD6E00
	;; (This saves the need to mess with mapping/unmapping the sector
	;; buffer).

	;; Get address to save to
	lda ptr1+0
	sta copysectorbuffer_destaddr+0
	lda ptr1+1
	sta copysectorbuffer_destaddr+1

	;; Execute DMA job
	lda #$00
	sta $d702
	sta $d704
	lda #>dmalist_copysectorbuffer
	sta $d701
	lda #<dmalist_copysectorbuffer
	sta $d705
		
	;; Retrieve the return value
	lda tmp2
	ldx tmp3
	RTS	

	.data

dmalist_copysectorbuffer:
	;; Copy $FFD6E00 - $FFD6FFF down to low memory 
	;; MEGA65 Enhanced DMA options
        .byte $0A  ;; Request format is F018A
        .byte $80,$FF ;; Source is $FFxxxxx
        .byte $81,$00 ;; Destination is $FF
        .byte $00  ;; No more options
        ;; F018A DMA list
        ;; (MB offsets get set in routine)
        .byte $00 ;; copy + last request in chain
        .word $0200 ;; size of copy is 512 bytes
        .word $6E00 ;; starting at $6E00
        .byte $0D   ;; of bank $D
copysectorbuffer_destaddr:	
        .word $8000 ;; destination address is $8000
        .byte $00   ;; of bank $0
        .word $0000 ;; modulo (unused)

	
	.code
	
_open:

        ;; Get pointer to file name
        ldy #0
	jsr cc65_args_read_ptr1_16
	jsr cc65_copy_ptr1_string_to_0100
	jsr setname_0100
	
	;; Actually call open
	LDA #$18
	STA $D640
	NOP
	LDX #$00
	RTS

_close:
	TAX
	LDA #$20
	STA $D640
	NOP
	LDX #$00
	RTS
	
