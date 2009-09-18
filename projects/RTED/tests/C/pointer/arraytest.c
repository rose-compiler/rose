#define N 10
int main(int argc, char* argv)
{
  
   int c;
   int i;
   struct aab {
       char * nm;
       int arrayA[N];
   } ss;

   for(i=0; i<N; i++)
       ss.arrayA[i] = i;
   i = i - N - 1;
   c = ss.arrayA[i];  /* read one element before the lower bound */


   return 0;
}

