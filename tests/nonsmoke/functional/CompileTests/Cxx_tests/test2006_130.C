/*
Hi Dan,

I have found an unusual case where the 'const' keyword is introduced
more than once.  This does not really have any practical impact as the
code still compiles but I thought I would mention it in case it might
be important to someone.

Input:
------------------------------------------------------------
typedef const int constint;

class A {

     static constint foo = 1;

};
------------------------------------------------------------

Output (resulting data member is const of a typedef which is a const type):
------------------------------------------------------------
typedef const int constint;

class A 
{
  private: static const constint foo = 1;
}

;
------------------------------------------------------------

The relevant code is in
src/frontend/SageIII/astFixup/fixupInClassDataInitialization.C and should
be an easy fix.

Thanks,
-- Peter 
*/

#error "Output is incorrect but it does compile (const normalization)"

typedef const int constint;
class A
   {
     public:
          static constint foo = 1;
   };

