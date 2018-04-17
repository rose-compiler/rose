;;; nasm -f elf i386-fld.s
;;; gcc -m32 -nostdlib -o i386-fld i386-fld.o
	
_start:
	fld	st0

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	