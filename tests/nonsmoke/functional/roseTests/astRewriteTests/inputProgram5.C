// This input code is used to test the innsertion of comments into the AST

void foo()
   {
     int x;
     for (int i=0; i < 0; i++)
        {
          for (int j=0; j < 0; j++)
             {
               int y;
             }
        }
   }

int main()
   {
     return 0;
   }
