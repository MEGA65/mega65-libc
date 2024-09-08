; To see how LLVM-MOS encodes this file, run:
;
;    llvm-mc -mcpu=mos45gs02 --show-encoding fileio.s
;
HYPPO_GETVERSION = $00; Output: A, X, Y, Z. Clear Z before exiting!
HYPPO_CHDIR      = $0C
HYPPO_OPENDIR    = $12; Output: A=file descriptor
HYPPO_READDIR    = $14; Input: X=file descriptor; Y MSB of destination
HYPPO_CLOSEDIR   = $16; Input: X=file descriptor
HYPPO_OPENFILE   = $18
HYPPO_READFILE   = $1A
HYPPO_CLOSEFILE  = $20
HYPPO_CLOSEALL   = $22
HYPPO_SETNAME    = $2E
HYPPO_FINDFILE   = $34
HYPPO_CDROOTDIR  = $3C
HYPPO_TOGGLE_ROM_WRITE_PROTECT = $70
FILE_ERROR       = $FF
NAME_ERROR       = $FF
HTRAP00          = $D640; Hypervisor trap, register A

.macro hyppo hyppo_cmd
	lda #\hyppo_cmd
	sta HTRAP00
	clv
.endmacro

.section .text.fileio_copy_rc2,"ax",@progbits
copy_rc2_string_to_0100:
	; copy file name
	ldy #0
copyloop:
	lda (__rc2), y
	sta $0100, y
	iny
	cmp #0
	bne copyloop
	rts	

setname_0100:
	ldy #>$0100
	ldx #<$0100
	hyppo HYPPO_SETNAME
	bcs setname_ok
	lda #NAME_ERROR
setname_ok:
	rts
	
.global closeall
.section .text.fileio_closeall,"ax",@progbits
closeall:
	hyppo HYPPO_CLOSEALL
	rts

.global toggle_rom_write_protect
.section .text.fileio_toggle_rom_write_protect,"ax",@progbits
toggle_rom_write_protect:
	hyppo HYPPO_TOGGLE_ROM_WRITE_PROTECT
	rts

.global read512
.section .text.fileio_read512,"ax",@progbits
read512:
	hyppo HYPPO_READFILE; outputs bytes read -> X, Y
	stx __rc4
	sty __rc5

	; ensure SD buffer is selected, not FDC buffer
	lda #$80
	tsb $D689
	
	; Copy the full 512 bytes from the sector buffer at $FFD6E00
	; (This saves the need to mess with mapping/unmapping the sector
	; buffer).
	; Input buffer pointer in rc2 and rc3
	lda __rc2
	sta copysectorbuffer_destaddr+0
	lda __rc3
	sta copysectorbuffer_destaddr+1

	; do DMA job
	lda #$00
	sta $D702
	sta $D704
	lda #>dmalist_copysectorbuffer
	sta $D701
	lda #<dmalist_copysectorbuffer
	sta $D705

	lda __rc4; return file length word ...
	ldx __rc5; ... through A, X
	rts

.section .data.fileio_dmalist_copysectorbuffer
dmalist_copysectorbuffer:
	; Copy $FFD6E00 - $FFD6FFF down to low memory 
	; MEGA65 Enhanced DMA options
	.byte $0A  ;; Request format is F018A
	.byte $80,$FF ;; Source is $FFxxxxx
	.byte $81,$00 ;; Destination is $FF
	.byte $00  ;; No more options
	; F018A DMA list
	; (MB offsets get set in routine)
	.byte $00 ;; copy + last request in chain
	.short $0200 ;; size of copy is 512 bytes
	.short $6E00 ;; starting at $6E00
	.byte $0D   ;; of bank $D
copysectorbuffer_destaddr:	
	.short $0000 ;; destination address
	.byte $0   ;; of bank $0
	.short $0000 ;; modulo (unused)

.global open
.section .text.fileio_open,"ax",@progbits
open:
	jsr copy_rc2_string_to_0100
	jsr setname_0100
	hyppo HYPPO_FINDFILE
	bcs findfile_ok
	lda #FILE_ERROR
	rts
findfile_ok:
	hyppo HYPPO_OPENFILE; outputs to A
	rts

.global close
.section .text.fileio_close,"ax",@progbits
close:
	tax;
	hyppo HYPPO_CLOSEFILE; input: X
	rts

.global chdirroot
.section .text.fileio_chdirroot,"ax",@progbits
chdirroot:
	hyppo HYPPO_CDROOTDIR
	rts

.global chdir	
.section .text.fileio_chdir,"ax",@progbits
chdir:
	jsr copy_rc2_string_to_0100
	jsr setname_0100	
	hyppo HYPPO_FINDFILE
	bcs chdir_ok
	lda #FILE_ERROR
	rts
chdir_ok:
	hyppo HYPPO_CHDIR
	hyppo HYPPO_OPENFILE; outputs to A
	rts

.global gethyppoversion
.section .text.fileio_gethyppoversion,"ax",@progbits
gethyppoversion:
    hyppo HYPPO_GETVERSION ; outputs to Q = A, X, Y, Z
    stq (__rc2)            ; store Q to __rc2 pointer
    ldz #0                 ; Z must be cleared before returning
    rts
