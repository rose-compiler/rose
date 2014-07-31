;;; nasm -f elf pentium-xmm.s
;;; gcc -m32 -nostdlib -o pentium-xmm pentium-xmm.o
	
_start:
	movdqa	xmm0, xmm1
	palignr	xmm0, xmm1, 3

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	