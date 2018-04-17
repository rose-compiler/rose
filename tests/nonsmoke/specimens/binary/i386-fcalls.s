;;; nasm -f elf i386-fcalls.s
;;; gcc -m32 -nostdlib -o i386-fcalls i386-fcalls.o

_start:
	call T1
	call T2
	call T3
	call T4
	call T5
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

;;; Call a thunk.  We have nested thunks, and no matter which we call, callee should ultimately return here
;;; rather than to the thunks.
T5:	call T5_1
	call T5_2
	call T5_3
	ret
	
T5_1:	jmp callee
T5_2:	jmp T5_1
T5_3:	jmp T5_2

;;; The thing that everything else is calling. Make it two nodes so its more like a typical function and
;;; that way we can see the entry node vs. the return node.
callee:	nop
	ret
	
