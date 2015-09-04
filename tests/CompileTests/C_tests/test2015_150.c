// Example code from Xen.

int foobar()
   {
     if (1)
        {
          switch (42)
             {
               default:
                    return -( { __asm__ ( ".section \".einfo\", \"\", @progbits\n\t" ".align 8\n\t" "\n1:\n\t" ".long ( 4f - 1b )\n\t" ".long %c0\n\t" ".long ( 2f - 1b )\n\t" ".long ( 3f - 1b )\n\t" ".long %c1\n\t" "\n2:\t.asciz \"" "Invalid argument" "\"\n\t" "\n3:\t.asciz \"" "drivers/net/ath/ath5k/test2015_06.c" "\"\n\t" ".align 8\n\t" "\n4:\n\t" ".previous\n\t" : : "i" ( ( ( 0x1c << 24 ) | ( ( 0x00004000 | 0x00500000 ) ) | ( 0 << 8 ) | ( 0x0002 << 0 ) ) ), "i" ( 38 ) ); ( ( 0x1c << 24 ) | ( ( 0x00004000 | 0x00500000 ) ) | ( 0 << 8 ) | ( 0x0002 << 0 ) ); } );
             }
        }
       else
          if (7)
             {
             }
     return 0;
   }
