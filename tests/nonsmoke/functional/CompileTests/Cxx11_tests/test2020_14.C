
enum A : char;
enum class B;

void foobar()
   {
  // Unparsed as: sizeof(enum A  : char );
  // Original code is: sizeof(A);
     sizeof(A);

  // Unparsed as: sizeof(enum class B );
  // Original code is: sizeof(B);
     sizeof(B);
   }

