;;; nasm -f elf i386-pxor.s
;;; gcc -m32 -nostdlib -o i386-pxor i386-pxor.o
	
_start:
	pxor	mm4, mm5
	pxor	xmm0, xmm1

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	