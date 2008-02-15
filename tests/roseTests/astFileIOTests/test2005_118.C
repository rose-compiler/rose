// Example of where a non-defining member function declaration is constructed within the AST
// To support this better, member functions should include a member function to return a 
// boolean for if the defining declaration appears in the class or outside the class.

class A 
   {
  // This declaration should result in one non-defining declaration and one defining declaration.
     public:
       // Calling f2 within definition of f1 (andbefore f2 is declared)
          void f1()
             {
            // Builds a non-definingfunction declaration for use by the function call (puts it into the symbol table)
               f2();
             }

       // Reuses the non-defining declaration in construction of forward declaration of member function
          void f2();
   };

// Build the defining declaration (should build a new defining declaration)
void A::f2()
   {
     int x;
     int y;
   }

class B
   {
  // This declaration should result in one defining declaration and NO non-defining declaration.
     public:
       // Calling f2 within definition of f1 (andbefore f2 is declared)
          void f1()
             {
            // Builds a non-definingfunction declaration for use by the function call (puts it into the symbol table)
               f2();
             }

       // Reuses the non-defining declaration in construction of the defining declaration of member function
          void f2()
             {
               int x;
               int y;
             }
   };

#if 0
// This is not legal C++
class C
   {
  // This declaration should result in one defining declaration and NO non-defining declaration.
     public:
       // Calling f2 within definition of f1 (andbefore f2 is declared)
          void f1()
             {
            // Builds a non-definingfunction declaration for use by the function call (puts it into the symbol table)
               f2();
             }

       // Can't have a namespace inside of a class (I'm happy for that)
          namespace X
             {
            // Reuses the non-defining declaration in construction of the defining declaration of member function
               void f2();
                  {
                    int x;
                    int y;
                  }
             }
   };
#endif



