// DQ (7/20/2004): This bug is suspected to be related to the current problem that Qing has
// with an assertion failure in the unparser.

// This bug demonstrates a case where in the unparser, a class declaration is 
// found to have a parent which is a SgClassType!  Since it is not a stateent,
// the parent appears to be NULL and fails an assertion test. I can only assume 
// that this is an error since it makes no sense to me.

class A
   {
     public:
          void foo () {};
   };

// Commenting this out causes the problem to go away!
typedef void (A::*PointerToMemberFunctionType)();


// This class declaration causes a class declaration to have a parent which is a SgClassType!
// This would seem to be VERY wrong, and it causes an assertion failure in the unparser.
class B
   {
     public:
          B() {};
          A* pointerToObject;
   };

