
#define __NR_rt_sigreturn    173

#  define _MY_SIGRETURN(name) \
   ".text\n" \
   ".globl my_sigreturn\n" \
   "my_sigreturn:\n" \
   "	movl	$" #name ", %eax\n" \
   "	int	$0x80\n" \
   ".previous\n"

#define MY_SIGRETURN(name)  _MY_SIGRETURN(name)
#if 0
/* MSVC modes does not handle asm statements */
asm(
   MY_SIGRETURN(__NR_rt_sigreturn)
);
#endif

extern void my_sigreturn(void);

int main ()
   {
     void (*function_pointer)(void);

     function_pointer = my_sigreturn;

     return 0;
   }

