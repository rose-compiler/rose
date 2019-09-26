template <  typename T_Type2, typename ... T_Types3>
void func2 ( T_Type2 parm1, T_Types3 ... parms2){}

void func1()
   {
  // func2< int >(0, [=] () {}, [=] () {});

  // Original code is: func2< int >(0, [=] () {}, 1, 2);
  // Unparsed as:    ::func2< int, int, int > (0, [=] (){},1,2);
     func2< int >(0, [=] () {}, 1, 2);
   }

