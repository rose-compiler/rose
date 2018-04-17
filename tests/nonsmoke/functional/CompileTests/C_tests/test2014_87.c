#define __NR_rt_sigreturn    173

#if 1
// This is the original example of the code that now generates good code
// using a special version of the asm_escapeString() function.
#define _MY_SIGRETURN(name) \
   ".text\n" \
   ".globl my_sigreturn\n" \
   "my_sigreturn:\n" \
   "	movl	$" #name ", %eax\n" \
   "	int	$0x80\n" \
   ".previous\n"
#else
// This is a bad example that generated malformed code when expaned.
#define _MY_SIGRETURN(name) \
   ".text " \
   ".globl my_sigreturn " \
   "my_sigreturn: " \
   "movl $" #name ", %eax " \
   "int $0x80 " \
   ".previous"
#endif

#define MY_SIGRETURN(name)  _MY_SIGRETURN(name)

asm( MY_SIGRETURN(__NR_rt_sigreturn) );
