int main() 
   {
     int i;
     auto x = [&i]()
        {
          i = 2;
        };
   }

