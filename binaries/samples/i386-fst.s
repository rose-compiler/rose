;;; nasm -f elf i386-fst.s
;;; gcc -m32 -nostdlib -o i386-fst i386-fst.o
	
_start:
	fstp	st1
	fst	st1

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	