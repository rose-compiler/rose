struct myStruct
   {
     int var_a;
     int && reference_to_var_a;
     myStruct(int input_var, int&& input_reference_to_var)
        : var_a(input_var), reference_to_var_a(static_cast<int&&>(input_reference_to_var))
        {
        }
   };

void foobar()
   {
     int var_x = 42;
     myStruct struct_var (var_x, static_cast<int&&>(var_x));
   }
