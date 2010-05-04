// Multi-byte NOP instructions.
// Note: I can't seem to get the memonic versions to work properly
#define NOP_1_BYTE asm ("nop")
#define NOP_2_BYTE asm (".byte 0x66,0x90")
#define NOP_3_BYTE asm ("nopl (%eax)")
#define NOP_4_BYTE asm ("nopl 0x01(%eax)")
#define NOP_5_BYTE asm (".byte 0x0f,0x1f,0x44,0x00,0x00")
#define NOP_6_BYTE asm (".byte 0x66,0x0f,0x1f,0x44,0x00,0x00")
#define NOP_7_BYTE asm (".byte 0x0f,0x1f,0x80,0x00,0x00,0x00,0x00")
#define NOP_8_BYTE asm (".byte 0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00")
#define NOP_9_BYTE asm (".byte 0x66,0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00")

#if 0
  uses eax dependencies (Intel-recommended choice)
1: nop
2: osp nop
3: nopl (%eax)
4: nopl 0x00(%eax)
5: nopl 0x00(%eax,%eax,1)
6: osp nopl 0x00(%eax,%eax,1)
7: nopl 0x00000000(%eax)
8: nopl 0x00000000(%eax,%eax,1)
#endif


void foo()
   {
  // Test of multi-byte NOP's.
  // Note that it is reported at:
  //    http://kerneltrap.org/mailarchive/linux-kernel/2008/9/16/3313714/thread
  // that some emulators don't handle 32-bit nopl instructions well.
  // This is not important for our tests here unless we translate nops sleds to
  // multi-byte nops and expect to run the executable.

  // 1 byte nop
     asm ("nop"); // (unparses to "nop")
     asm (".byte 0x90");

  // 2 byte nop
     asm ("nop"); // (unparses to "nop")
     asm (".byte 0x66,0x90");

  // 3 byte nop
     asm ("nopl (%eax)"); //  (unparses to "nop DWORD PTR ds:[eax]")
     asm (".byte 0x0f,0x1f,0x00");
     asm (".byte 0x66,0x66,0x90"); // (unparses to "nop")

  // 4 byte nop
     asm ("nopl 0x04(%eax)"); // (unparses to "nop DWORD PTR ds:[eax + 0x04]")
     asm (".byte 0x0f,0x1f,0x40,0x04");
     asm (".byte 0x66,0x66,0x66,0x90"); // (unparses to "nop")

  // 5 byte nop
     asm ("nopl 0x05(%eax,%eax,0x01)"); //  (unparses to "nop DWORD PTR ds:[eax + eax + 0x05]")
     asm (".byte 0x0f,0x1f,0x44,0x00,0x05");
     asm (".byte 0x66,0x66,0x66,0x66,0x90"); // (unparses to "nop")

  // 6 byte nop
     asm ("nopl 0x06(%eax,%eax,1)"); // (only 5 bytes unparses to "nop DWORD PTR ds:[eax + eax + 0x06]")
     asm (".byte 0x66,0x0f,0x1f,0x44,0x00,0x06"); // unparses to "nop WORD PTR ds:[eax + eax + 0x06]")
     asm (".byte 0x66,0x66,0x66,0x66,0x66,0x90"); // (unparses to "nop")

  // 7 byte nop 
     asm ("nopl 0x00000007(%eax)"); // (only 4 bytes unparses to "nop DWORD PTR ds:[eax + 0x07]")
     asm (".byte 0x0f,0x1f,0x80,0x00,0x00,0x00,0x07"); // (unparses to "nop DWORD PTR ds:[eax + 0x07000000]")
     asm (".byte 0x66,0x66,0x66,0x66,0x66,0x66,0x90"); // (unparses to "nop")

  // 8 byte nop 
     asm ("nopl 0x00000008(%eax,%eax,0x01)"); // (only 5 bytes unparses to "nop DWORD PTR ds:[eax + eax + 0x08")
     asm (".byte 0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x08"); // unparses to "nop DWORD PTR ds:[eax + eax + 0x08000000]")
     asm (".byte 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x90"); // (unparses to "nop")

  // 9 byte nop
     asm ("nopl 0x00000009(%eax,%eax,1)"); // (only 5 bytes unparses to "nop DWORD PTR ds:[eax + eax + 0x09")
     asm (".byte 0x66,0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x09");// unparses to "nop WORD PTR ds:[eax + eax + 0x09000000]")
     asm (".byte 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x90"); // (unparses to "nop")
   }


int main()
   {
     int result = 0;
     result++;
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");

     result++;
     return result;
   }
