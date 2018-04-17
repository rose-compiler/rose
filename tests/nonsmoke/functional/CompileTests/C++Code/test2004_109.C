
class X
   {
   };

class Y : public X
   {
     static int y;
   };

int Y::y = 42;

class Z : public X
   {
     int z;
     void noninlinePublicMemberFunctionWithoutDefinition ();
   };

// No inline specifier was used in the declaration in the class so none appears here
void Z::noninlinePublicMemberFunctionWithoutDefinition () {}


#if 1
class classTypeWithPureVirtualMember
   {
     public:
          virtual void publicPureVirtualMemberFunction () = 0;
   };

class classTypeWithoutBaseClass
   {
     public:
          int x;
   };

class classTypeWithPublicBaseClass : public classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithProtectedBaseClass : protected classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithPrivateBaseClass : private classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithVirtualPublicBaseClass : virtual public classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithVirtualProtectedBaseClass : virtual protected classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithVirtualPrivateBaseClass : virtual private classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithoutBaseClassA { public: int x; };
class classTypeWithoutBaseClassB { public: int y; };
class classTypeWithoutBaseClassC { public: int z; };
class classTypeWithMultipleVirtualBaseClasses : virtual public    classTypeWithoutBaseClassA,
                                                virtual protected classTypeWithoutBaseClassB,
                                                virtual private   classTypeWithoutBaseClassC
   {
     public:
          int a;
   };
#endif

#if 1
template < class T >
class templateClassType
   {
     typename T::X x;
   };
#endif

