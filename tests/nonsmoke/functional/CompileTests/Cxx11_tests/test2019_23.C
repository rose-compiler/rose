template <  typename T_Type2, typename ... T_Types3>
void func2 ( T_Type2 parm1, T_Types3 ... parms2) {}

void func1()
   {
     func2< int >(0, [=] () {});
   }

