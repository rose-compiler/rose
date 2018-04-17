_start:
	sub	esp, 8

	;; Test that writing a 32-bit register to memory and then
	;; reading it back results in the same value as originally.
	mov	[esp], eax
	mov	ecx, [esp]
	
	;; Test that reading from an uninitialized memory
	;; location has an effect similar to writing to the memory
	;; and then reading it.  In other words, the value stored
	;; in EAX should be a simple variable, and memory should
	;; contain four bytes extracted from that variable.
	mov	eax, [esp-4]

	;; exit
	mov	eax, 1
	int	0x80
	hlt
	