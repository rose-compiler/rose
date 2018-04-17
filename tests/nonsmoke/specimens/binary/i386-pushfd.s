;;; nasm -f elf i386-pushfd.s
;;; gcc -m32 -nostdlib -o i386-pushfd i386-pushfd.o
	
_start:
	sub ax, 0x34		; instantiates some flag bits
	pushfd			; should not coalesce flag bits

	mov ebx, eax		; should coalesce al into eax
	hlt
