// Unnamed structure or union members do not participate in initialization and have 
// indeterminate value after initialization. Therefore, in the following example, 
// the bit field is not initialized, and the initializer 3 is applied to member b.
struct {
     int a;
     int :10;
     int b;
     } w = { 2, 3 };

