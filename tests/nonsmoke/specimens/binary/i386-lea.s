;;; nasm -f elf i386-lea.s && gcc -m32 -nostdlib -o i386-lea i386-lea.o

_start:
	lea ax, [esp]
	lea eax, [esp]
	hlt
	