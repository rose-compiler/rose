// Example C++ 11 features:

int main() 
   {
     bool result = false;

  // Simple example of lambda function in for loop (C++11 specific feature).
     for (int i=0; i < 5; [&result] () { if (result == false) result = true; })
        {
       // for loop body
        }

     return 0;
   }
