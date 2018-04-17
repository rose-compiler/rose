BITS 32

_start:
	cmp eax, ebx
	je .else
	mov byte [esp+4], 1
	jmp .end
.else:  mov byte [esp+5], 2
.end:	ret
