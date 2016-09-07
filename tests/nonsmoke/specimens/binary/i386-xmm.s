;;; nasm -f elf i386-xmm.s
;;; gcc -m32 -nostdlib -o i386-xmm i386-xmm.o
	
        movdqa    xmm1, xmm7
        pslldq    xmm7, 4
        pxor      xmm7, xmm1
        movdqa    xmm1, xmm7
        pslldq    xmm7, 8
        pxor      xmm7, xmm1
	hlt
