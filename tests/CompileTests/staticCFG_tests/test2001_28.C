// Bug associated with Overture code GenericGrid.h (line 105)
// Assertion failed: get_symbol_ptr(type->source_corresp) != NULL, 
//      file /home/dquinlan/ROSE/NEW_ROSE/EDG/src/sage_il_to_str.C, line 2160

class A
   {
     public:
          A* foo () { return 0; }
          A* Aptr;
   };

class B
   {
     A* foo () { return Aptr->foo(); }
     A* Aptr;
   };

