.global _opendir, _readdir, _closedir	

HYPPO_GETVERSION = $00;
HYPPO_CHDIR = $0C;
HYPPO_OPENDIR = $12;; output file descriptor in A
HYPPO_READDIR = $14; Input: X=file descriptor; Y MSB of destination
HYPPO_CLOSEDIR = $16; file handle in X
HYPPO_OPENFILE = $18;
HYPPO_READFILE = $1A;
HYPPO_CLOSEFILE = $20;
HYPPO_CLOSEALL = $22;
HYPPO_SETNAME = $2E;
HYPPO_FINDFILE = $34;
HYPPO_TOGGLE_ROM_WRITE_PROTECT = $70;
FILE_ERROR = $FF;

.section code,"a"

; closedir takes file descriptor from X
_closedir:
	tax
	lda #HYPPO_CLOSEDIR
	sta $D640
	clv
	rts
	
; Opendir takes no arguments and returns File descriptor in A
_opendir:
	lda #HYPPO_OPENDIR
	sta $D640
	clv
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
_readdir:
	pha
	;; First, clear out the dirent
	ldx #0
	txa
l1:	sta _readdir_dirent, x	
	dex
	bne l1

	;; Third, call the hypervisor trap
	;; File descriptor gets passed in in X.
	;; Result gets written to transfer area we setup at $0400
	plx
	ldy #>$0400 		; write dirent to $0400 
	lda #HYPPO_READDIR
	sta $D640
	clv

	bcs readDirSuccess
	sta __rc2
	ldx __rc3
	rts

readDirSuccess:
	;;  Copy file name
	ldx #$3f
l2:	lda $0400, x
	sta _readdir_dirent + 4 + 2 + 4 + 2, x
	dex
	bpl l2
	;; make sure it is null terminated
	ldx $0400 + 64
	lda #$00
	sta _readdir_dirent + 4 + 2 + 4 + 2, x

	;; Inode = cluster from offset 64 + 1 + 12 = 77
	ldx #$03
l3:	lda $0477, x
	sta _readdir_dirent + 0, x
	dex
	bpl l3

	;; d_off stays zero as it is not meaningful here	
	;; d_reclen we preload with the length of the file (this saves calling stat() on the MEGA65)
	ldx #3
l4:	lda $0400 + 64 + 1 + 12 + 4, x
	sta _readdir_dirent + 4 + 2, x
	dex
	bpl l4

	;; File type and attributes
	;; XXX - We should translate these to C style meanings
	lda $0400 + 64 + 1 + 12 + 4 + 4
	sta _readdir_dirent + 4 + 2 + 4

	;; Return address of dirent structure
	lda #<_readdir_dirent
	ldx #>_readdir_dirent
	rts

.data
_readdir_dirent:
	.short 0,0   	; d_ino
	.short 0		; d_off
	.short 0,0		; d_reclen
	.short 0		; d_type
	.space 256, 0x0
