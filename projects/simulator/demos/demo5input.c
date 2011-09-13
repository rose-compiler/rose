/* Executes something on the stack */

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

static int
payload()
{
    write(1, "pawned\n", 7);
    exit(1);
}

int
main()
{
    char buf[32];

    /* Make sure the stack is executable */
    mprotect((void*)((unsigned)buf & ~0xfff), 0x1000, PROT_READ|PROT_WRITE|PROT_EXEC);

    /* Put some instructions on the stack */
    buf[0] = 0xb8;                              /* mov eax, $payload */
    *(unsigned*)(buf+1) = (unsigned)payload;
    buf[5] = 0xff;                              /* jmp eax */
    buf[6] = 0xe0;

    /* Jump to those instructions */
    asm("jmp *%0" :: "r" (buf));

    return 0;
}
