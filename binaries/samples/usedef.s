;;; These are various unit tests for the simple use-def analysis in BinaryAnalysis::InstructionSemantics2::SymbolicSemantics.
;;; Compile this on an intel machine with "nasm -f elf32 usedef.s && gcc -m32 -nostdlib -o usedef usedef.o"

_start:	je case1
end:	hlt
	
	;; Moving one register to another does not modify the definers for the value that was moved
case1:  mov eax, ebx
	mov eax, eax
	je end

	;; Binary operators add CI to each result
case2   add eax, eax
	add ebx, ebx
	add eax, ebx
	je end

	;; CI is always added to new constants
case3: 	mov eax, 1
	je end

	;; Simplifications don't change the results
case4:  mov eax, 3
	mov ebx, 4
	add eax, ebx
	neg ecx			;only here so next branch is unknown
	je end

	;; Reading and writing subregisters doesn't affect the definers
case5:	mov eax, ebx
	mov ah, bh
	mov ah, bl
	mov ax, cx
	mov eax, ebx
	je end

	;; Writing bytes to memory does not affect the definers of the bytes written
case6:	mov [eax], ebx
	je end

	;; Reading bytes from memory does not affect the definers of the bytes read
case7:	mov eax, [ebx]
	je end

	;; Is repeated logical right shift simplified?
case8:
	shr eax, 10		;eax is originally an unknown value
	shr eax, 10
	shr eax, 10
	shr eax, 2		;eax should be zero after this
	mov eax, ebx		;so the predicate isn't opaque
	je end

	;; XOR of a register with itself is an idiom for clearing the register. The result should not
	;; depend on the operands
case9:  mov eax, 1
	xor eax, eax
	neg ecx			;only here so next branch is unknown
	je end

	;; Pushing constants onto the stack
case10:	push 3
	mov eax, 7
	push eax
	je end

	hlt
