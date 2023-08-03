.global closeall, open, close, read512, toggle_rom_write_protect, chdir, chdirroot

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
HYPPO_TOGGLE_ROM_WRITE_PROTECT = $70
FILE_ERROR       = $FF
NAME_ERROR       = $FF

.macro hyppo hyppo_cmd
	lda #\hyppo_cmd
	sta $D640
	clv
.endmacro

.section code,"a"
    
copy_rc2_string_to_0100:	
	; copy file name
	phy
	ldy #0
NameCopyLoop:
	lda (__rc2),y
	sta $0100,y
	iny
	cmp #0
	bne NameCopyLoop
	ply
	rts	

setname_0100:
	ldy #>$0100
	ldx #<$0100
	lda #HYPPO_SETNAME
	sta $D640
	clv
	; XXX Check for error (carry would be clear)
	bcs setname_ok
	lda #NAME_ERROR
setname_ok:
	rts
	
closeall:
	lda #HYPPO_CLOSEALL
	sta $D640
	clv
	rts

toggle_rom_write_protect:
	lda #HYPPO_TOGGLE_ROM_WRITE_PROTECT
	sta $D640
	clv
	rts
	
read512:
	;  Get pointer to buffer
	lda __rc2
	ldx __rc3
	; Select current file
	; XXX - Not currently implemented
	; Read the next sector of data
	lda #HYPPO_READFILE
	sta $D640
	clv
	; bytes read returned in X and Y; stash these for returning
	stx __rc4
	sty __rc5

	; Make sure SD buffer is selected, not FDC buffer
	lda #$80
	tsb $D689
	
	; Copy the full 512 bytes from the sector buffer at $FFD6E00
	; (This saves the need to mess with mapping/unmapping the sector
	; buffer).
	; Get address to save to
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

	; Return file length word through A and X
	lda __rc4
	ldx __rc5
	rts

.data
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

.section code,"a"
open:
    ; argument pointer stored in rc2 and rc3
	lda __rc2
	ldx __rc3
	
	jsr copy_rc2_string_to_0100
	jsr setname_0100	

	lda #HYPPO_FINDFILE
	sta $D640
	clv
	bcs open_file_exists
	lda #FILE_ERROR
	rts
open_file_exists:
	lda #HYPPO_OPENFILE; outputs to A
	sta $D640
	clv
	rts

close:
	tax
	lda #HYPPO_CLOSEFILE; input in X
	sta $D640
	clv
	rts

chdirroot:
	lda #$3c; this seems undocumented as of July 2023?
	sta $D640
	clv
	ldx #$00
	rts
	
chdir:
    ; Get pointer to file name
	lda __rc2
	ldx __rc3
	
	jsr copy_rc2_string_to_0100
	jsr setname_0100	

	lda #HYPPO_FINDFILE
	sta $D640
	clv
	bcs chdir_file_exists
	lda #FILE_ERROR
	rts
chdir_file_exists:	
	lda #HYPPO_CHDIR
	sta $D640
	clv
	lda #HYPPO_OPENFILE; outputs to A
	sta $D640
	clv
	rts
