;;; nasm -f elf i386-mul.s
;;; gcc -m32 -nostdlib -o i386-mul i386-mul.o
        
_start:
        mov     eax, ebx
        mov     edx, ebx
        mul     esi

        mov     eax, ebx
        mov     edx, ebx
        mul     ecx

        mov     eax, ebx
        mov     edx, ebx
        mul     dword [esp+0x14]

        ;; exit
        mov     eax, 1
        int     0x80
        hlt
        