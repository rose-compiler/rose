// This test code demonstrates: special case of ck_ptr_to_member

class BaseOutputDocInterface
   {
     public:
       // Note that startTitle() must be declared in a base class of OutputGenerator.
          void startTitle();
   };

class OutputGenerator : public BaseOutputDocInterface
   {
   };

class OutputList
   {
     public:
          void forall(void (OutputGenerator::*func)());

          void foobar() 
             {
            // This should be an address of a SgMemberFunctionRefExp (not the same thing as a member function call).
               forall(&OutputGenerator::startTitle);
             }
   };
