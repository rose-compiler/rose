
int main() {

   struct aab {
       char * nm;
       int b;
   } ss[ 10 ], *ptr;

   ptr = ss;
   // ptr now points outside of ss, we have left the array, and even the same
   // memory region
   ptr += 10;

   // illegal read of p
   int x = ptr -> b;

   return 0;
}
