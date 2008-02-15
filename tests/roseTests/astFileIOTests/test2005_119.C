// Example of use of static variable before declaration,
// need to build the declaration only once.

struct S
{ bool foo () { return y; }
  
  static bool y;
};

