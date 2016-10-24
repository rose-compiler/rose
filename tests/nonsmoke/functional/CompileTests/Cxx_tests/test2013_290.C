class Test_2nd_Part_Of_Type
   {
  // This is the secondary declaration and it was a bug that it 
  // would be given a source position from the primary declaration.
     static Test_2nd_Part_Of_Type* PointerArray [10];
   };

// This is the primary declaration (it's source position was always correct).
Test_2nd_Part_Of_Type* Test_2nd_Part_Of_Type::PointerArray [10];
