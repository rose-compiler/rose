// This is from test2011_115.C (but specific to an error introduced in the newest name qualificaiton).
namespace X
   {
     const int const_size = 5;
   }

struct Z
   {
  // Here the bit with specifier requires name qualification.
     int fieldVar_Z:(X::const_size+X::const_size*2);
   };

