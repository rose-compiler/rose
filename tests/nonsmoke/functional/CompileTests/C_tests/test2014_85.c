
// __asm__(".text\n.globl my_sigreturn\nmy_sigreturn:\n\tmovl\t$173, %eax\n\tint\t$0x80\n.previous\n");
__asm__(".text\n.globl my_sigreturn\nmy_sigreturn:\n\tmovl\t$173, %eax\n\tint\t$0x80\n.previous");

/* __asm__(".text\n.globl my_sigreturn\nmy_sigreturn_version_2:\n\tmovl\t$174, %eax\n\tint\t$0x80\n.previous\n"); */

int main()
   {
     return 0;
   }

