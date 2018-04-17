// number #34

typedef enum enumType1{};

class Array
   {
     public:
       // Error: This becomes a SgEnumDeclaration in the AST 
       // Is this a typedef which has no name?
#if 1
       // Within this scope ::enumType1 is hidden (should be in EDG's list of hidden names)
          typedef enum enumType1{};
#endif
//        Array(enumType1 x);
          Array(::enumType1 x);
   };
