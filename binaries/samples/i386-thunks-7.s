;;; Test specific thunk situation

_start:
	push 16
	call thunk1
	hlt

thunk2:
	jmp dword [foo_got]

thunk1:
	jmp thunk2

SECTION .rodata
	foo_got:
	dd 0xaaa0000					; not mapped
