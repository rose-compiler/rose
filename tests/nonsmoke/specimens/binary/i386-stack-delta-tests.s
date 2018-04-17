;;; nasm -f elf i386-stack-delta-tests.s
;;; gcc -m32 -nostdlib -o i386-stack-delta-tests i386-stack-delta-tests.o

;;; Unit tests for stack delta analysis.  The function names end with "_pN" to indicate a positive stack delta of N (i.e.,
;;; the function's effect is to add N to the ESP register), "_nN" to indicate a negative stack delta of N, "_top" to
;;; indicate that the stack delta is not a constant, or "_na" to indicate that stack delta is not applicable (e.g., the
;;; function never returns).
	
_start: jmp dispatch

;; Very simple test. Stack delta should be +4
test1_p4:
	ret

;;; Another simple test
test2_p4:
	push eax
	push eax
	pop eax
	pop eax
	ret

;;; Push and pop different sizes
test3_p4:
	push eax
	push ax
	pop eax
	pop ax
	ret

;;; Manipulate the stack pointer directly
test4_p4:
	sub esp, 16
	add esp, 8
	add esp, 8
	ret

;;; See if subtracting and then adding an unknown quantity works
test5_p4:
	sub esp, eax		; eax is unknown
	add esp, eax		; undo the subtract
	ret

;;; Subtract and add equal but unknown amounts in parts
test6_p4:
	sub esp, eax		; subtract unknown amount
	add eax, 10
	add esp, eax		; add back, but overshoot by 10
	sub esp, 10		; subtract the extra 10
	ret

;;; "if/else" and both sides have same net effect
test7_p4:
	je .else
	push eax
	pop eax
	jmp .endif
.else:	push ax
	pop ax
.endif: ret

;;; "if/else" with different efects
test8_top:
	je .else
	push eax
	pop eax
	jmp .endif
.else:	push eax
.endif:	ret

;;; stack delta analysis must terminate for an infinite loop, although the result
;;; is not applicable since the function cannot return.
test9_na:
.top:
	jmp .top
	;; not reached

;;; calling an unknown function means we don't know our own stack delta
test10_top:
	call eax
	ret

;;; calling an unknown function in two different branches also results in TOP
test11_top:
	je .else
	call eax
	jmp .endif
.else:  call eax
.endif: ret

;;; calling an unknown function in an infinite loop must terminate
test12_top:
.top:
	call eax
	jmp .top

;;; calling an unknown function in a (terminating) loop must terminate and result is TOP
test13_top:
.top:	test eax, eax
	je .done
	call eax
	inc eax
	jmp .top
.done:	ret

;;; calling an unkonwn function on a path that doesn't return does not affect the result
test14_p4:
	test eax, eax
	je .endif
.loop:	call eax
	jmp .loop
.endif:	ret

;;; branching to an unknown address results in TOP because we cannot prove that flow of control doesn't enter
;;; back into our own function with some arbitrary stack delta.
test15_top:
	test eax, eax
	je .endif
	jmp eax			;this could branch to: sub esp, 1; jmp .endif
.endif: ret

;;; storing and retrieving the stack pointer from memory works within a single basic block
test16_p4:
	mov ebp, esp
	mov [ebp-4], esp
	sub esp, 0x100
	mov esp, [ebp-4]
	ret

;;; storing and retrieving the stack pointer in memory does not work across basic blocks. This was a conscious
;;; decision make the algorithm faster by not needing to compute variable addresses (another analysis that can be rather
;;; slow) and which also simplifies the stack-delta analysis by not requiring it to consider aliasing effects.
test17_top:
	mov ebp, esp
	mov [ebp-4], esp
	sub esp, 0x100
	test eax, eax
	je .endif
	inc eax			;to prevent the "je" from being optimized away by ROSE
.endif: mov esp, [ebp-4]	;stack pointer is okay before this insn, but gets lost after
	ret

;;; storing the stack delta into a register that could be clobbered by a function call.  This assumes no information
;;; about the calling convention and ROSE doesn't yet try to figure it out.
test18_helper:	ret
test18_top:
	push ebp
	mov ebp, esp
	call test18_helper	;ebp is clobbered (as is most everything else)
	mov esp, ebp
	pop ebp
	ret

;;; This is last so that adding more calls doesn't change the address of existing functions, thus
;;; making it easier to check for regressions.
dispatch:	
	call test1_p4
	call test2_p4
	call test3_p4
	call test4_p4
	call test5_p4
	call test6_p4
	call test7_p4
	call test8_top
	je .skip9
	call test9_na		;never returns
	jmp 0			;so a mistake is highly visible in the CFG
.skip9:	call test10_top
	call test11_top
	call test12_top
	call test13_top
	call test14_p4
	call test15_top
	call test16_p4
	call test17_top
	call test18_top
	hlt
