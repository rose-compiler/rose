;;; nasm -f elf i386-dataflow.s && gcc -nostdlib -m32 -o i386-dataflow i386-dataflow.o


;;; Dataflow tests
_start:	jmp start

global1:
	RESD 1

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else where both sides do the same thing
ifTest1:
	test eax, eax
	je .L1
	
	mov ebx, 1
	jmp .L2
.L1:
	mov ebx, 1
.L2:
	;; assert ebx == 1
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else test where the two sides do different things
ifTest2:
	test eax, eax
	je .L1

	mov ebx, 1
	jmp .L2
.L1:
	mov ebx, 2
.L2:
	;; assert ebx == unknown
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else test where both sides do the same thing but the ECX register is
;;; read for the first time in both sides.
ifTest3:
	test eax, eax
	je .L1

	mov ebx, ecx
	jmp .L2
.L1:
	mov ebx, ecx
.L2:
	;; assert ebx == ecx
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Conditional assignment
ifTest4:
	test eax, eax
	je .L1

	mov ebx, 1
.L1:
	;; assert ebx == unknown
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop whose body is executed at least once and always does the same thing.
do1:
.loop:
	mov ebx, 1
	cmp eax, 10
	jne .loop

	;; assert ebx == 1
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop whose body is executed at least once, doing something different each
;;; time.
do2:
.loop:
	mov ebx, eax
	cmp eax, 10
	jne .loop

	;; assert ebx == 1
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop that makes the same assignment every time
while1:
.loop:
	cmp eax, 10
	je .end

	mov ebx, 1
	inc eax
	jmp .loop
.end:
	;; assert ebx == unknown
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop that makes a different assignment every time
while2:
.loop:
	cmp eax, 10
	je .end

	mov ebx, eax
	inc eax
	jmp .loop
.end:
	;; assert ebx == unknown
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Conditional assignment to a program argument
ifMem1:
	test eax, eax
	je .endif

	mov dword [esp+4], 1
.endif:
	;; assert arg_4 == 1
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else statement that makes the same assignment on both sides
ifMem2:
	test eax, eax
	je .else

	mov dword [esp+4], 1
	jmp .endif
.else:
	mov dword [esp+4], 1
.endif:
	;; assert arg_4 == 1
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else statement that makes a different assignment on each side
ifMem3:
	test eax, eax
	je .else

	mov dword [esp+4], 1
	jmp .endif
.else:
	mov dword [esp+4], 2
.endif:
	;; assert arg_4 == unknown
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop whose body is executed at least once and which always does the
;;; same thing.
doMem1:
.loop:
	mov dword [esp+4], 1
	cmp eax, 10
	jne .loop

	;; assert arg_4 == 1
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop whose body is executed at least once and which does something
;;; different each time.
doMem2:
.loop:
	inc ebx
	mov dword [esp+4], ebx
	cmp eax, 10
	jne .loop

	;; assert arg_4 == unknown
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop that makes the same assignment every time, but might not be executed.
whileMem1:
.loop:
	cmp eax, 10
	je .end

	mov dword [esp+4], 1
	inc eax
	jmp .loop
.end:
	;; assert arg_4 == unknown
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Conditional assignment to a program argument
ifGlobal1:
	test eax, eax
	je .endif

	mov dword [global1], 1
.endif:
	;; assert arg_4 == 1
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else statement that makes the same assignment on both sides
ifGlobal2:
	test eax, eax
	je .else

	mov dword [global1], 1
	jmp .endif
.else:
	mov dword [global1], 1
.endif:
	;; assert arg_4 == 1
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An if/else statement that makes a different assignment on each side
ifGlobal3:
	test eax, eax
	je .else

	mov dword [global1], 1
	jmp .endif
.else:
	mov dword [global1], 2
.endif:
	;; assert arg_4 == unknown
	ret

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop whose body is executed at least once and which always does the
;;; same thing.
doGlobal1:
.loop:
	mov dword [global1], 1
	cmp eax, 10
	jne .loop

	;; assert arg_4 == 1
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop whose body is executed at least once and which does something
;;; different each time.
doGlobal2:
.loop:
	inc ebx
	mov dword [global1], ebx
	cmp eax, 10
	jne .loop

	;; assert arg_4 == unknown
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; A loop that makes the same assignment every time, but might not be executed.
whileGlobal1:
.loop:
	cmp eax, 10
	je .end

	mov dword [global1], 1
	inc eax
	jmp .loop
.end:
	;; assert arg_4 == unknown
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
stackAlloc1:
	push ebp
	mov ebp,esp
	mov eax, 0
.loop:
	cmp eax, 10
	jz .end

	push eax
	inc eax
	jmp .loop
.end:
	mov esp, ebp
	pop ebp
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
stackAlloc2:
	push ebp
	mov ebp, esp
	mov eax, 0
.loop:
	cmp eax, 10
	jz .end

	sub esp, 4
	mov dword [esp], eax
	inc eax
	jmp .loop
.end:
	mov esp, ebp
	pop ebp
	ret
	
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
start:
	call ifTest1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifTest2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifTest3
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifTest4
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call do1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call do2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call while1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifMem1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifMem2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifMem3
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call doMem1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call doMem2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call whileMem1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifGlobal1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifGlobal2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call ifGlobal3
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call doGlobal1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call doGlobal2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call whileGlobal1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call stackAlloc1
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call stackAlloc2
	mov eax, [esp+4]
	mov ebx, [esp+8]
	
	call while2
	hlt
