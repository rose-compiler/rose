;;; nasm -f elf i386-mixed.s
;;; gcc -m32 -nostdlib -o i386-mixed i386-mixed.o
;;;
;;; Tests data mixed in between instructions

_start:
        jmp .b+4
.b:     dd 0xdeadbeef
        hlt
        