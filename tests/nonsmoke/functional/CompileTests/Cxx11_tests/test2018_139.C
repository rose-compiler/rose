struct foo {int a; char b[2];} structure;

int main(int argc, char **argv) 
   {
     structure = ((struct foo) {5, 'a', 0});

     return 0;
   }
