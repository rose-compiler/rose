

template < typename T_Type1, typename T_Type2, typename ... T_Types3>
void func2 ( T_Type2 && parm1, T_Types3 && ... parms2) {}

class Class1 {};

void func1()
   {
     func2< Class1 >(0, [=] () { });
   }

