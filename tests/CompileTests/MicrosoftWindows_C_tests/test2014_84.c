#define __NR_rt_sigreturn    173

#define _MY_SIGRETURN(name) \
   ".text\n" \
   ".globl my_sigreturn\n" \
   "my_sigreturn:\n" \
   "	movl	$" #name ", %eax\n" \
   "	int	$0x80\n" \
   ".previous\n"

#define MY_SIGRETURN(name)  _MY_SIGRETURN(name)

#if 0
/* MSVC mode does not handle asm statements */
asm( MY_SIGRETURN(__NR_rt_sigreturn) );
#else
/* Some versions of C do not allow an empty file (need to check on MSVC). */
int x;
#endif
