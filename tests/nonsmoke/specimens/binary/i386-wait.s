;;; nasm -f elf i386-wait.s
;;; gcc -m32 -nostdlib -o i386-wait i386-wait.o
	
_start:
	wait

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	