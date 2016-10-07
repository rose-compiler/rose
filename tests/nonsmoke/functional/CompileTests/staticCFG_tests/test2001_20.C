// Demonstration of bug in SAGE

// This is an internal SAGE error demonstration test code
//    Error: problem with function call using member function pointer.
//
// SAGE calls the SgExpression::set_type() function when 
// processing this code but the SgExpression::set_type() is a virtual
// member function which is not ment to be called.  So there should be
// SgDotStart::set_type() member function implemented.  But no such
// SgDotStart::set_type() member function is implemented in the original 
// SAGE II.
//
// Problem code construct:
// (Operand_X.Original_Referenced_Array->*Function_Pointer) ();

class A
   {
     public:
          void foo () {};
   };

typedef void (A::*PointerToMemberFunctionType)();

class B
   {
     public:
          B() {};
          A* pointerToObject;
   };

class C
   {
     private:
     // const B X; is unparsed as class B const X; 
        B X;
        PointerToMemberFunctionType memberFunctionPointer;

     public:
        void Execute();
   };

#if 0
// Comment out until we fix this
void
C::Execute ()
   {
  // unparsed as:       *(this -> X).pointerToObject .* this -> memberFunctionPointer(); 
  // corrected version: ((*X.pointerToObject).*memberFunctionPointer)(); 
  // better modified version: (*(this->X).pointerToObject.*(this->memberFunctionPointer)) ();
     (X.pointerToObject->*memberFunctionPointer)();
   }
#endif






