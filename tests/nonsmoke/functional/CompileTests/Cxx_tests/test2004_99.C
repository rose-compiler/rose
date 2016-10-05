// This test code demonstrates the overly shared use of 
// class declarations which cause the separate class
// declarations to be represented internally as a single 
// IR node which is redundently listed in the global scope.
// This test code is part of the testing of the definin vs. 
// nondefining delcaration handling within ROSE.

// ERROR: The multiple forward declarations cause the test for 
//    unique statements in a scope to fail.  
// SOLUTION: Explicitly allow new IR nodes
//    to be generated so that each forward declaration will
//    be a unique declaration.

class X;
class X;

class X* (*foo)(void);

class X {};

