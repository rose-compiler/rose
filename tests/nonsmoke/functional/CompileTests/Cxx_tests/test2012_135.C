
int main() 
   {
     bool result = false;

     for (int i=0; i < 5; [&result] () { if (result == false) result = true; })
        {
       // for loop body
        }

     return 0;
   }
