struct struct1
   { 
     int array1[2]; 
   };

struct struct2
   { 
     struct1 array1[2]; 
   };

// This is a struct of array of structs which forces the C++11 specific initialization.
// And that appears to be the issue that forces the class names to be output.
// AggregateInitializer:Class --> AggregateInitializer:Array --> AggregateInitializer:Class --> AssignmentInitializer
// The class name is required when we output the initialization of the structure.
struct2 local2 = { struct1 {1, 1} , struct1 {1, 1} };
// struct2 local2 = { {1, 1} , {1, 1}};

// This is an array of structs, having a non C++11 specific inistilizer.
// Unparses as:
// struct1 local3 [2] = {struct1 {1, 1} , struct1 {1, 1}};
// AggregateInitializer:Array --> AggregateInitializer:Class --> AggregateInitializer:Array --> AssignmentInitializer
// The class name is NOT required when we output the initialization of the array of structure.
struct1 local3 [2] = { {1, 1} , {1, 1} };
