;;; nasm -f elf pentium-movd.s
;;; gcc -m32 -nostdlib -o pentium-movd pentium-movd.o
	
_start:
	movd	mm0, [esp+4]
	movd	[0x40001234], mm0

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	