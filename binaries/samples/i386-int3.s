;;; nasm -f elf i386-int3.s
;;; gcc -m32 -nostdlib -o i386-int3 i386-int3.o
	
_start:
	int 3

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	