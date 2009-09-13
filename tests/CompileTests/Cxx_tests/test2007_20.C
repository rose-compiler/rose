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
/* 
  Large complicated asm command.
  Error: "fx" ans "fsw" appear to be undefined in the generated code.
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

   asm volatile ("fnsave %[fx]" :: "i" ((0 * 32 + 24)));
*/
// asm volatile ("661:\n\t" "fnsave %[fx] ;fwait;" ".byte 0x90\n" :: "i" ((0 * 32 + 24)),	[fx] "m" (tsk->thread.i387.fxsave));
// asm volatile ("fnsave %[fx];" :: "i" ((0 * 32 + 24)),	[fx] "m" (tsk->thread.i387.fxsave));
#ifdef ROSE_USE_NEW_EDG_INTERFACE
// Simpler case of new syntax (now supported by EDG 4.0 and in ROSE).
   asm volatile ("fnsave %[fx];" :: [fx] "m" (tsk->thread.i387.fxsave));
#endif

#if 0
// Saved original version (same as below)
  asm volatile ("661:" "fnsave %[fx] ;fwait;" ".byte 0x90"
		".byte 0x8d,0xb4,0x26,0x00,0x00,0x00,0x00"
		".byte 0x8d,0x74,0x26,0x00 " "662:"
		".section .altinstructions,a" "  .align 4"
		" .long 661b" "  .long 663f" "  .byte %c0"
		" .byte 662b-661b" "  .byte 664f-663f " ".previous"
		" .section .altinstr_replacement, ax " "663:"
		" fxsave %[fx] " "bt $7,%[fsw] ; jnc 1f ; fnclex1:"
		" 664: " " .previous" : : "i" ((0 * 32 + 24)),
		[fx] "m" (tsk->thread.i387.fxsave),
		[fsw] "m" (tsk->thread.i387.fxsave.swd):"memory");
#endif

#ifdef ROSE_USE_NEW_EDG_INTERFACE
  asm volatile ("661:" "fnsave %[fx] ;fwait;" ".byte 0x90"
		".byte 0x8d,0xb4,0x26,0x00,0x00,0x00,0x00"
		".byte 0x8d,0x74,0x26,0x00 " "662:"
		".section .altinstructions,a" "  .align 4"
		" .long 661b" "  .long 663f" "  .byte %c0"
		" .byte 662b-661b" "  .byte 664f-663f " ".previous"
		" .section .altinstr_replacement, ax " "663:"
		" fxsave %[fx] " "bt $7,%[fsw] ; jnc 1f ; fnclex1:"
		" 664: " : : "i" ((0 * 32 + 24)),
		[fx] "m" (tsk->thread.i387.fxsave),
		[fsw] "m" (tsk->thread.i387.fxsave.swd):"memory");
#endif
}



int
main ()
{


}
