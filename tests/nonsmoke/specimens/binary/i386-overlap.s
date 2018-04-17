;;; nasm -f elf i386-overlap.s
;;; gcc -m32 -nostdlib -o i386-overlap i386-overlap.o
;;;
;;; Tests instructions that overlap
;;; Control flow is like this:
;;;
;;;   basic block:
;;;     0: mov eax, 0xbbc10300          -> 5
;;;
;;;   basic block:
;;;     5: mov ecx, 0x05000000          -> a
;;;     a: add eax, ecx                 -> c
;;;     c: jmp 1                        -> 1
;;;     1: add byte ds:[ebx], al        -> 3
;;;     3: sar dword ds:[ebx+0xb9], 5   -> a

_start:
;; +0
        mov eax, 0xbbc10300        ; b8 00 03 c1 bb
;; +1   add byte ds:[ebx], al      ;    00 03
;; +3   sar dword ds:[ebx+0xb9], 5 ; c1 bb b9 00 00 00 05
;; +5
        mov ecx, 0x05000000        ; b9 00 00 00 05
;; +a
        add eax, ecx               ; 01 c8
;; +c
        jmp _start+1               ; e9 f0 ff ff ff

