/*

Later comment from Rich:
     Addendum to my earlier bug report: The fact that 'enum bar' below is 'private' 
     is not actually relevant; the important feature is that it is declared after 
     its values are used in this class.

The following example causes an assertion failure:

   class Foo {
       public:
           Foo () : x_ (A) {}
       private:
           enum bar {A, B};
           bar x_;
   };

The failure:

   ...src/frontend/SageIII/astPostProcessing/fixupDefiningAndNondefiningDeclarations.C:107: virtual void FixupAstDefiningAndNondefiningDeclarations::visit(SgNode*): 
Assertion "declaration == definingDeclaration" failed.

Making the enum "public" works fine.

Is this construct illegal C++? I vaguely you saying this (or something like it) is illegal, 
but that g++ accepts it. Anyway, I encountered this construct in KULL, so I guess we should 
handle it. (Brian, I saw this in KULL-39039, where my translator failed.)

--rich
*/

class Foo
   {
     public:
          Foo () : x_ (A) {}
     private:
          enum bar {A, B};
          bar x_;
   };
