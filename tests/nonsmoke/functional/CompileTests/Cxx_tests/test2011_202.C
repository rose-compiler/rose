
class X
   {
     public:
          X(int & __a) { }
   };

// This test demonstrates that the base class is not recorded properly in the AST.
class allocator: public X
   {
     public:
       // Demonstrate case of direct base class in preinitialization list.
          allocator(int & __a) : X(__a) { }        
   };

