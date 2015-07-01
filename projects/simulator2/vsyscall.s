BITS 64

;;; at address 0
;;; void vgettimeofday((struct timeval * tv, struct timezone * tz)
vgettimeofday:
	mov rax, 96					; __NR_gettimeofday
	syscall
	ret
	resb 0x3f3


;;; at address 0x400
;;; time_t vtime(time_t *t)
vtime:
	mov rax, 201					; __NR_time
	syscall
	ret
	resb 0x3f3

;;; at address 0x800
;;; long vgetcpu(unsigned *cpu, unsigned *node, struct getcpu_cache *tcache)
vgetcpu:
	mov rax, -38					; -ENOSYS
	ret
	resb 0x3f5

;;; at address 0c00
;;; long venosys_1(void)
vnosys_1:
	mov rax, -38					; -ENOSYS
	ret
	resb 0x3f5
