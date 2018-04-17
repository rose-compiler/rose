;;; nasm -f elf i386-stmxcsr.s
;;; gcc -m32 -nostdlib -o i386-stmxcsr i386-stmxcsr.o
	
_start:
	stmxcsr	[0x04000000]

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	