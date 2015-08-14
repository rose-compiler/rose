;;; Specimen source code for tests/roseTests/binaryTests/cory006.C
;;; Functions with code in between.

_start:	push ebp
	mov ebp, esp
	nop
	leave
	ret

	mov ecx, eax
loop_a:	dec ecx
	je _start
	jmp loop_a

	int3

	mov ecx, eax
loop_b:	dec ecx
	je _start
	jmp loop_b

func1:	push ebp
	mov ebp, esp
	nop
	leave
	ret

	nop
	nop
	nop

func2:	push ebp
	mov ebp, esp
	nop
	leave
	ret
