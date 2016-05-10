;;; Tests SymbolicSemantics::RiscOperators::readMemory using lazily instantiated initial state.
BITS 32

;;; int foo(int choice, int x) {
;;;     int retval;
;;;     if (choice) {
;;;         retval = x;
;;;     } else {
;;;         retval = x;
;;;     }
;;;     return retval;
;;; }

foo:	push ebp
	mov ebp, esp
	;; if (choice)
	mov ebx, dword [ebp+8]
	jne .else
	;; retval = x
	mov eax, dword [ebp+12]
	jmp .endif
.else:
	;; retval = x
	mov eax, dword [ebp+12]
.endif:
	leave
	ret
