#if 0
A related bug to earlier bug posted with regards to template
instantiations with character arguments.

Compiling the following code with ROSE:
template <char sep>
class ReadContainer2
{ public:
  static bool read(){
  return 0;}
  };

class
ParameterDeclaration
{
public:
bool
read(
)
   {
      return ReadContainer2<','>::read();
   }
};

gives the following error:
identityTranslator: Cxx_Grammar.C:58210: SgClassDeclaration*
SgClassSymbol::get_declaration() const: Assertion "this != __null" failed.
#endif


template <char sep>
class ReadContainer2
   {
     public:
          static bool read()
             {
               return 0;
             }
   };

class ParameterDeclaration
   {
     public:
          bool read()
             {
               return ReadContainer2<','>::read();
             }
   };
