typedef enum { one, two } *enum_type, enum_type2;

void foobar()
   {
     enum_type number;
     *number = two;
   }
