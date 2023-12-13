;;; Comments start with semicolons.
;;; Registers are named r0 through r255 and are 16 bits wide
;;; Some registers have special names:
;;;     pc - program counter is the address of the current instruction
;;;     sp - the stack pointer
;;;     fp - points to a function call frame on the stack
;;;     lr - link register holds return address for a function call
;;; Labels are defined as "FOO:" and used as "$FOO" and are 16 bit values
;;; Most lines are instructions taking zero or more comma-separated operands
;;; Some lines are assembler directives:
;;;     .pc = VALUE    - sets the address for the next instruction

	.pc = 0x1000
	
f1:	push fp                 ; push the frame pointer
        push lr                 ; push the link register
        mov sp, fp              ; addr of new call frame
        load 0x4321, r0		; r0 = 0x4321

	call $f2
        pop lr
        pop fp
        ret

	;; Sum values from 1 to N, inclusive, where N is the first argument (r16)
sum:	push fp
	push lr
	load 0, r0		;r0 is the sum and return value
	load 0, r1		;r1 is the loop counter
	jmp $sum_test
sum_loop:	
	add r1, r0
sum_test:
	cmp r1, r16		;compare counter with first argument
	load 1, r3
	add r3, r1		;increment loop counter
	blt $sum_loop		;loop until counter reaches argument
	pop lr
	pop fp
	ret

f2:	
        push fp
        push lr
	mov sp, fp
        load 16, r0
	call $sum
        pop lr
        pop fp
        ret
        
