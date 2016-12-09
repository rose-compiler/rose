
class X
   {
     public:
          virtual void Get_Lhs_Array_ID() = 0;
   };

// It is a bug to output the "= 0" for a pure virtual function outside of the class where it is declared.
void X::Get_Lhs_Array_ID()
   {
   }

#if 1
class Y : public X
   {
     public:
          virtual void Get_Lhs_Array_ID() = 0;
   };
#endif

#if 1
// It is a bug to output the "= 0" for a pure virtual function outside of the class where it is declared.
void Y::Get_Lhs_Array_ID()
   {
   }
#endif
   
