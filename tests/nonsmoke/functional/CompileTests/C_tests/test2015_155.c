
/* REAL_CODE: declare a fragment of code that executes in real mode */
#define REAL_CODE( asm_code_str )	\
	".code16\n\t"			\
	asm_code_str "\n\t"		\
	".code16gcc\n\t"

static unsigned int bios_attr = 0;

static void bios_putchar ( int character ) 
   {
     int discard_a, discard_b, discard_c;

  /* Print character with attribute */

  /* DQ (5/24/2015): EDG reports error for this asm statement: 
     " error: register "bp" has a fixed purpose and may not be clobbered in an asm statement"
   */

// In the Xen source code these failing cases are commented out (this is an EDG issue).
// #ifndef USE_ROSE
     __asm__ __volatile__ ( REAL_CODE ( "sti\n\t"
					   /* Skip non-printable characters */
					   "cmpb $0x20, %%al\n\t"
					   "jb 1f\n\t"
					   /* Set attribute */
					   "movw $0x0001, %%cx\n\t"
					   "movb $0x09, %%ah\n\t"
					   "int $0x10\n\t"
					   "\n1:\n\t"
					   /* Print character */
					   "xorw %%bx, %%bx\n\t"
					   "movb $0x0e, %%ah\n\t"
					   "int $0x10\n\t"
					   "cli\n\t" )
			       : "=a" ( discard_a ), "=b" ( discard_b ),
			         "=c" ( discard_c )
			       : "a" ( character ), "b" ( bios_attr )
			       : "ebp" );
  // #endif
   }
