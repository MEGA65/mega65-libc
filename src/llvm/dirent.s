; To see how LLVM-MOS encodes this file, run:
;
;    llvm-mc -mcpu=mos45gs02 --show-encoding dirent.s
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
HYPPO_CHDIRROOT  = $3C; undocumented as of July 2023?
HYPPO_TOGGLE_ROM_WRITE_PROTECT = $70
FILE_ERROR       = $FF
DST_DIRENT       = $0400

.macro hyppo hyppo_cmd
	lda #\hyppo_cmd
	sta $D640
	clv
.endmacro

.global closedir
.section .text.closedir,"ax",@progbits
closedir:
	tax
	hyppo HYPPO_CLOSEDIR; input X=file descriptor
	rts

.global opendir
.section .text.opendir,"ax",@progbits
opendir:
	hyppo HYPPO_OPENDIR; output A=file descriptor
	rts

; readdir takes the file descriptor returned by opendir as argument
; and gets a pointer to a MEGA65 DOS dirent structure.
; Again, the annoyance of the MEGA65 Hypervisor requiring a page aligned
; transfer area is a nuisance here. We will use $0400-$04FF, and then
; copy the result into a regular C dirent structure
;
; d_ino = first cluster of file
; d_off = offset of directory entry in cluster
; d_reclen = size of the dirent on disk (32 bytes)
; d_type = file/directory type
; d_name = name of file
.global readdir
.section .text.readdir,"ax",@progbits
readdir:
	pha                 ; store file descriptor on stack
	ldx #0              ; zero out dirent
	txa
l1:	sta _readdir_dirent, x
	dex
	bne l1
	plx                 ; pull file descriptor from stack into X
	ldy #>DST_DIRENT 	; MSB of destination dirent
	hyppo HYPPO_READDIR ; input X=file descriptor, Y MSB of destination
	bcs readdir_ok
	lda #0              ; return NULL pointer if error
	sta __rc2
	sta __rc3
	rts
readdir_ok:
	; Copy file name
	ldx #$3f
l2:	lda DST_DIRENT, x
	sta _readdir_dirent + 4 + 2 + 4 + 2, x
	dex
	bpl l2
	; ensure it is null terminated
	ldx DST_DIRENT + 64
	lda #$00
	sta _readdir_dirent + 4 + 2 + 4 + 2, x

	; Inode = cluster from offset 64 + 1 + 12 = 77
	ldx #$03
l3:	lda $0477, x
	sta _readdir_dirent + 0, x
	dex
	bpl l3

	;; d_off stays zero as it is not meaningful here
	;; d_reclen we preload with the length of the file (this saves calling stat() on the MEGA65)
	ldx #$03
l4:	lda DST_DIRENT + 64 + 1 + 12 + 4, x
	sta _readdir_dirent + 4 + 2, x
	dex
	bpl l4

	;; File type and attributes
	;; XXX - We should translate these to C style meanings
	lda DST_DIRENT + 64 + 1 + 12 + 4 + 4
	sta _readdir_dirent + 4 + 2 + 4

	; pointer to dirent structure returned through rc2, rc3
	lda #<_readdir_dirent
	ldx #>_readdir_dirent
	sta __rc2
	stx __rc3
	rts

.section .data.dirent
_readdir_dirent:
	.short 0,0   	; d_ino
	.short 0		; d_off
	.short 0,0		; d_reclen
	.short 0		; d_type
	.space 256, 0x0
