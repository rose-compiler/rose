package c_code_h is
   procedure c_func;
   pragma Import (C, c_func);

   procedure c_ada_caller;
   pragma Import (C, c_ada_caller);
end c_code_h;
