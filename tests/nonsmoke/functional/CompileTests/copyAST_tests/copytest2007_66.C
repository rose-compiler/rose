struct A
   {
     A(int i);

     operator bool()
        {
          return false;
        }
   };

int main()
   {
     if (A a = 2)
	     {
	     }
   }
