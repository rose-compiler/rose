// This is currently an error in ROSE (generates "using globalIntegerType::globalIntegerType;")
// I think the problem is with the name qualification mechanism

// namespace X {

typedef int globalIntegerType;

namespace A
   {
  // This is a using declaration (nonmember using-declaration)
  // using X::globalIntegerType;
     using ::globalIntegerType;
   }

// }
