
typedef long header_type;

extern "C"
   {

     struct __ATerm
        {
          header_type   header;
          union _ATerm *next;
        };

     typedef union _ATerm
        {
          header_type     header;
          struct __ATerm  aterm;
          union _ATerm*   subaterm[1];
       // MachineWord     word[1];
        } *ATerm;

   } // extern "C"

namespace ATERM
   {

     extern "C"
        {
          struct __ATerm
             {
               header_type   header;
               union _ATerm *next;
             };

          typedef union _ATerm
             {
               header_type     header;
               struct __ATerm  aterm;
               union _ATerm*   subaterm[1];
        //     MachineWord     word[1];
             } *ATerm;

        } // extern "C"

} // end of namespace


// Aterm x1;
// ATERM::Aterm x2;

void foo1 (ATerm x) 
   {
   }

void foo2 (ATERM::ATerm x)
   {
   }

int main( int argc, char* argv[])
   {
   }
