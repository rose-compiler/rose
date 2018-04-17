#if 0

// DQ (1/13/2006): I have fixed the problem in src/midend/programAnalysis/OAWrap/SAGE2OA.h
// but also included the test code that Andreas sent as an additional test code for ROSE.

I think this is something which should be changed in the ROSE code rather
than in the EDG parser. The function declaration overloaded in
src/midend/programAnalysis/OAWrap/SAGE2OA.h does arguably have the wrong
return type (char* instead of const char*), and both ICC and EDG gives the
same error.

Compiling the following code with ROSE:
class X{
public:
  virtual const char *GetSymNameFromSymHandle() = 0;
};

class Y: public X{
  public:
 char *GetSymNameFromSymHandle () {
    return 0;
  };
};
gives the following error:
"test.C", line 8: error: return type is not identical to nor covariant with
          return type "const char *" of overridden virtual function
function
          "X::GetSymNameFromSymHandle"
   char *GetSymNameFromSymHandle () {
         ^

Errors in EDG Processing!
#endif


class X
   {
     public:
          virtual const char *GetSymNameFromSymHandle() = 0;
   };

class Y : public X
   {
     public:
       // DQ: Source code should be changed so that return type matches the virtual function declaration
       // char *GetSymNameFromSymHandle ()
          const char *GetSymNameFromSymHandle ()
             {
               return 0;
             };
   };
