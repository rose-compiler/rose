; Assemble:	  nasm -f elf64 -l hello_64.lst  hello_64.asm
; Link:		    gcc -m64 -o hello_64  hello_64.o

; Declare needed C  functions
        global main		; the standard gcc entry point
        extern	printf		; the C function, to be called

section .data		; Data section, initialized variables
  msg:	db "Hello world", 10, 0	; C string needs 0

section .text       ; Code section.

main:				        ; the program label for the entry point
  push    rbp		    ; set up stack frame, must be aligned

	mov	    rdi, msg
  call    printf		; Call C function

	pop	    rbp		    ; restore stack

	mov	    rax, 0		; normal, no error, return value
	ret			; return

