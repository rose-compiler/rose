;;; nasm -f elf i386-pop.s
;;; gcc -m32 -nostdlib -o i386-pop i386-pop.o
        
_start:
	pop es			; pop 2 bytes
	pop eax			; pop 4 bytes

	push word [0x04000000]	; push 2 bytes
	push dword [0x04000000]	; push 4 bytes

        ;; exit
        mov     eax, 1
        int     0x80
        hlt
        