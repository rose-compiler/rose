// This bug is what prevents Cxx_Grammar.h from compiling (at least initially).


template<typename _Tp>
class vector
  {
  };

enum asm_register_name_enum
   {
   };

// AS(130607) This is the line that causes failure
// typedef vector<asm_register_name_enum> AsmRegisterNameList; 
typedef vector<asm_register_name_enum> AsmRegisterNameList; 

// Add a forward enum to see what happens
enum asm_register_name_enum;

