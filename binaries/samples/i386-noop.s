;;; Specimen for testing the BinaryAnalysis::NoOperation analysis.
;;; Those instructions marked with ";NOP" are part of a sequence of
;;; instructions that are equivalent to a NOP instruction (i.e.,
;;; they only affect the machine by changing the EIP register to
;;; a known value).
_start:
	jmp main		;NOP

;;; Plain old NOP instructions
test1:
	nop			;NOP
	mov eax, 0
	nop			;NOP
	nop			;NOP
	ret

;;; Exchange twice
test2:
	xchg eax, ebx		;NOP
	xchg eax, ebx		;NOP
	ret

;;; Jumps
test3:
	jmp .b			;NOP
.a:     jmp .c			;NOP
.b:     jmp .a			;NOP
.c:     ret

;;; Opaque predicate
test4a:
	mov eax, 0
	test eax, eax
	je .a			;NOP
	mov eax, 1
.a:     ret

;;; Non-opaque predicate
test4b:
	mov eax, [esp+100]	;something unknown
	test eax, eax
	je .a
	mov eax, 1
.a:	ret

;;; Push/pop
;;; A push/pop pair is a no-op if we assume normal stack semantics.
test5:
	push eax		;NOP
	pop eax			;NOP
	ret

;;; Repeated push/pop
test6:
	push eax		;NOP
	pop eax			;NOP
	push eax		;NOP
	pop eax			;NOP
	ret

;;; The pushf/popf pair is not a no-op since PUSHF zeros the VM and RF bits
;;; while POPF zeroes the VIP and VIF flags and preserves the old IOPL and VM bits.
test7:
	pushf
	popf
	ret

test8:
	pushad
	mov ebx, eax
	jmp .a
	mov eax, 0		;not reached
.a:
	popad
	ret

main:
	call test1
	call test2
	call test3
	call test4a
	call test4b
	call test5
	call test6
	call test7
	call test8
	hlt