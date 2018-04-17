/*
Example from attempt to compile Linux (by Andreas):

*/

struct i387_fxsave_struct
{
  unsigned short swd;
} __attribute__ ((aligned (16)));

union i387_union
{
  struct i387_fxsave_struct fxsave;
};

struct thread_struct
{
  union i387_union i387;
};

struct task_struct
{
  struct thread_struct thread;
};

static inline __attribute__ ((always_inline))
     void __save_init_fpu (struct task_struct *tsk)
{
  asm volatile ("661:\n\t" "fnsave %[fx] ;fwait;" ".byte 0x90\n"
		".byte 0x8d,0xb4,0x26,0x00,0x00,0x00,0x00\n"
		".byte 0x8d,0x74,0x26,0x00\n" "\n662:\n"
		".section .altinstructions,\"a\"\n" "  .align 4\n"
		"  .long 661b\n" "  .long 663f\n" "  .byte %c0\n"
		"  .byte 662b-661b\n" "  .byte 664f-663f\n" ".previous\n"
		".section .altinstr_replacement,\"ax\"\n" "663:\n\t"
		"fxsave %[fx]\n" "bt $7,%[fsw] ; jnc 1f ; fnclex\n1:"
		"\n664:\n" ".previous"::"i" ((0 * 32 + 24)),
		[fx] "m" (tsk->thread.i387.fxsave),
		[fsw] "m" (tsk->thread.i387.fxsave.swd):"memory");
}



int
main ()
{


}
