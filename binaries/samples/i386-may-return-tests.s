;;; nasm -f elf i386-may-return-tests.s
;;; gcc -m32 -nostdlib -o i386-may-return-tests i386-may-returntests.o

;;; Unit tests for may-return analysis.  Function names ending with "yes" may return to their caller,
;;; function name ending with "no" never return to their caller.

_start: jmp dispatch

;;; any calls to this function indicate an error
not_reached:
	ret

;;; most simple returning function
test1_yes:
	ret

;;; a function that calls a returning function also returns
test2_yes:
	call test1_yes
	ret

;;; simple non-returning function by virtue of an insn with no successors
test3_no:
	hlt

;;; simple non-returning function by virtue of infinite loop
test4_no:
	jmp test4_no

;;; function that only calls a non-returning function does not return
test5_no:
	call test4_no
	call not_reached
	ret			;try to fool anlysis into thinking we return

;;; calls a returning function and a non-returning function
test6_yes:
	test eax, eax
	je .else
	call test5_no
	call not_reached
	jmp .endif
.else:  call test1_yes
.endif: ret

;;;
test7_no:
	test eax, eax
	jge .loop
	hlt
.loop:	je .endif
	inc eax
.endif: jmp .loop
	

;;; calls a returning function in an infinite loop
test8_no:
	call test1_yes
top:	call test1_yes
	jmp top
	call not_reached
	ret

;;; calls a non-returning function in a loop
test9_yes:
	jmp .test
.loop:  cmp edi, 0xa
	jne .test
	call test3_no
.test:  inc edi
	cmp edi, edi
	je .loop
	ret

;;; why is may-return analysis important?
test10_demo:
	mov eax, 0
	inc eax
	inc eax
	inc eax
	inc eax
	inc eax
	cmp eax, 5
	jne .endif

	call test5_no
	;; None of the following is reached. It's only present to confuse disassemblers
	cmp eax, 1
	je test10_demo+1	;middle of an instruction
	inc eax
	je test10_demo+2	;ditto
	inc eax
	je test10_demo+3	;ditto
	inc eax
	je test10_demo+4	;ditto
	inc eax
	je test10_demo+5	;the first INC
	inc eax
	je test10_demo+6	;the second INC
	inc eax
	je test10_demo+7	;the third INC
	inc eax
	je test10_demo+8	;the fourth INC
	inc eax
	je test10_demo+9	;the fifth INC
.endif:	ret			;not reached

	
	


;;; This is last so that adding more calls doesn't change the address of existing functions, thus
;;; making it easier to check for regressions.
dispatch:
	test eax, eax
	je .skip1
	call test1_yes
.skip1: test eax, eax
	je .skip2
	call test2_yes
.skip2: test eax, eax
	je .skip3
	call test3_no
.skip3: test eax, eax
	je .skip4
	call test4_no
.skip4: test eax, eax
	je .skip5
	call test5_no
.skip5: test eax, eax
	je .skip6
	call test6_yes
.skip6: test eax, eax
	je .skip7
	call test7_no
.skip7: test eax, eax
	je .skip8
	call test8_no
.skip8: test eax, eax
	je .skip9
	call test9_yes
.skip9: test eax, eax
	je .skip10
	call test10_demo
.skip10:hlt
