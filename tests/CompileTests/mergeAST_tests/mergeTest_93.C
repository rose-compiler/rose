// This example demonstrated why unique names must include the "_parameter_of_defining_declaration" 
// or "_parameter_of_nondefining_declaration" substring to avoid having the SgVariableSymbol
// be lost from the defining function's function definition's symbol table.
template <typename T>
class X
   {
     public:
          virtual void do_toupper(const T & t) const = 0;
   };

inline
void
X<char>::do_toupper(const char & c) const
   {
   }

