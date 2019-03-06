
void foobar()
   {
     enum { color = 7 };
     struct color
        {
          int integer_value;
          enum { RED=42 };
        };

     color;

     color::RED;
   }	
