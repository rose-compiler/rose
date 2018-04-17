// This test code demonstrates a bug in rose.h that only happens in 
// a large code.  This is a smaller version of the failing code that 
// is 380K lines.  Delta debugging is still trying to identify a 
// reduced size code (after 11 days, so far).  

// DQ (2/12/2013): I guessed at what might be the problem.

class Disassembler
   {
     public:
#if 1
          void set_partitioner(class Partitioner *p)
#else
          void set_partitioner()
#endif
             {
               p_partitioner;
             }

     protected:
          class Partitioner *p_partitioner;
   };


