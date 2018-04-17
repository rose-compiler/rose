// This input code is used to test the insertion of include directives into the AST

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
