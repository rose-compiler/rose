;;; nasm -f elf fcalls.s
;;; gcc -m32 -nostdlib -o fcalls fcalls.o

_start:
	call T1
	call T2
	call T3
	call T4
	mov eax, 1
	int 0x80
	hlt

;;; A normal function call
T1:	call callee
	ret

;;; Semantics of a call instruction, but not using CALL
T2:	push .L1
	jmp callee
.L1:	ret

;;; Not a call since it never pushes a return address
T3:	jmp callee
	ret			;not reached

;;; More complex
T4:	push eax
	mov eax, .L1
	add eax, 2
	sub eax, 1
	push eax
	sub eax, 1
	push eax
	inc eax
	inc eax
	jmp callee
.L1	pop eax
	pop eax
	ret

	
callee:	ret
	
