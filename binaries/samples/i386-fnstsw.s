;;; nasm -f elf i386-fnstsw.s
;;; gcc -m32 -nostdlib -o i386-fnstsw i386-fnstsw.o
        
_start:
	;; Note that the FSTSW is the same opcode as FNSTSW but preceded by WAIT (0x9b)
	fnstsw ax
	fnstsw [0x04000002]

        ;; exit
        mov     eax, 1
        int     0x80
        hlt
        