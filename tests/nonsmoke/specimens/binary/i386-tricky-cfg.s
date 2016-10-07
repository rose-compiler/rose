;;; nasm -f elf i386-tricky-cfg.s
;;; gcc -m32 -nostdlib -o i386-tricky-cfg i386-tricky-cfg.o

;;; Disassembly that's tricky to get right
	
_start: jmp dispatch

;;; --------------------------------------------------------------------------------
;;; RET used as an unconditional branch instruction
retbranch1:
	mov edi, 1		;loop counter
	mov eax, 0		;sum
	push .L1
	ret			;this is not really returning from this function
.L2:	add eax, edi
	inc edi
.L1:	cmp edi, 1000000	;test loop exit condition
	jne .again
	ret
.again:	push ebx		;ebx is unkonwn
	add dword [esp], .L2
	sub dword [esp], ebx	;subtract the unknown ebx, leaving just .L2
	ret			;branch to .L2, top of loop

;;; --------------------------------------------------------------------------------
;;; Function calls using CALL instruction
func1:	inc eax			;something for us to call
	ret

normal_call:			;typical function call
	call func1
	ret

regind_call:			;register-indirect function call computed statically
	mov eax, func1
	call eax
	ret

memind_call:			;memory-indirect function call computed statically
	mov eax, func1
	push eax
	call dword [esp]
	pop eax
	ret

piece_call:			;computing the callee address in memory
	push ebx		;unknown1
	add dword [esp], 20	;+20
	sub dword [esp], func1	;-func1
	mov eax, 20
	push eax
	neg dword [esp]
	pop eax
	add dword [esp], eax	;-20
	sub dword [esp], ebx	;-unknown1
	pop ebx
	xchg eax, ebx
	neg eax
	call eax		;calls func1
	ret

misaligned_call:		;compute call address using misaligned memory
	mov eax, 0x34120000
	push eax
	mov eax, 0x7856
	push eax
	pop ax
	pop eax			;eax == 0
	pop bx
	add eax, .L1
	push eax
	jmp func1
.L1	ret
	

fcall_tests:
	call normal_call
	call regind_call
	call memind_call
	call piece_call
	call misaligned_call
	ret

;;; --------------------------------------------------------------------------------
;;; Function calls that don't use the CALL instruction

noncalls:
	push .L1
	jmp func1		;this is a function call
	
.L1	call .L2		;not a real function call
.L2:    pop edi			;get my own address
	add edi, 13		;address of INC below
	push edi
	jmp func1		;this is a function call that...
	inc eax			;returns to here

	mov eax, .L3
	add eax, ebx		;ebx is unknown
	sub eax, ebx		;eax == .L3 again
	push eax
	jmp func1		;this is a function call
.L3:	inc eax

	pop eax
	inc eax
	dec eax
	jmp eax			;this is the function return

	hlt			;not reached

;;; --------------------------------------------------------------------------------
;;; Parallel CFG edges

pedges:
	test eax, eax
	je .L1		    	;fall-through and branch are the same
.L1:	je .L2		    	;can be reached whether eax is zero or not
.L2:	je .L3			;similarly...
.L3:	je .L4			;all JE instructions are in the same basic block
.L4	je .L5
	inc eax		    	;reached when eax != 0
	jmp .L6
.L5:	dec eax			;reached when eax == 0
.L6:	ret
	
;;; --------------------------------------------------------------------------------
;;; This is last so that adding more calls doesn't change the address of existing functions, thus
;;; making it easier to check for regressions.
dispatch:	
	call retbranch1
	call fcall_tests
	call noncalls
	call pedges
	hlt
