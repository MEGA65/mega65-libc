
	.setcpu "65C02"
	.export _closeall, _open, _close, _read512, _toggle_rom_write_protect, _chdir, _chdirroot

	.include "zeropage.inc"
	
.SEGMENT "CODE"

	.p4510

mega65_io_enable:
	lda #$47
	sta $d02f
	lda #$53
	sta $d02f
	rts
	
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
	jsr mega65_io_enable
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
	jsr mega65_io_enable
	LDA #$22
	STA $D640
	NOP
	LDX #$00
	RTS

_toggle_rom_write_protect:
	jsr mega65_io_enable
	lda #$70
	STA $d640
	nop
	ldx #0
	rts
	
_read512:

	;;  Get pointer to buffer
	sta ptr1+0
	stx ptr1+1

	;; Select current file
	;; XXX - Not currently implemented
	
	;; Read the next sector of data
	jsr mega65_io_enable
	LDA #$1A
	STA $D640
	NOP
	LDX #$00

	;; Number of bytes read returned in X and Y
	;; Store these for returning
	stx tmp2
	sty tmp3

	;; Make sure SD buffer is selected, not FDC buffer
	lda #$80
	tsb $D689
	
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
	sta ptr1+0
	stx ptr1+1
	
	jsr cc65_copy_ptr1_string_to_0100
	jsr setname_0100	

	;; Find file
	; Look for file on FAT file system via hypervisor calls
	lda #$34
	sta $d640
	nop
	bcs open_file_exists

	;; No such file.
	lda #$ff
	tax
	rts

open_file_exists:	
	;; Actually call open
	jsr mega65_io_enable
	lda #$00
	sta $d640
	nop
	
	LDA #$18
	STA $D640
	NOP
	
	LDX #$00
	RTS

_close:
	TAX
	jsr mega65_io_enable
	LDA #$20
	STA $D640
	NOP
	LDX #$00
	RTS

_chdirroot:
	;; Change to root directory of volume

	lda #$3C
	sta $d640
	nop

	ldx #$00
	rts
	
_chdir:

        ;; Get pointer to file name
	sta ptr1+0
	stx ptr1+1
	
	jsr cc65_copy_ptr1_string_to_0100
	jsr setname_0100	

	;; Find file
	; Look for file on FAT file system via hypervisor calls
	lda #$34
	sta $d640
	nop
	bcs chdir_file_exists

	;; No such file.
	lda #$ff
	tax
	rts

chdir_file_exists:	
	;; Actually call chdir
	jsr mega65_io_enable
	lda #$0C
	sta $d640
	nop
	
	LDA #$18
	STA $D640
	NOP
	
	LDX #$00
	RTS
