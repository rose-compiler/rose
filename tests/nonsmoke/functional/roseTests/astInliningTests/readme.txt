A brief description for current test files:

The goal is to see what language features are covered and what should be added.

test1.C  // direct call as a single statement, accessing a global variable
test1A.C  // additionally call an externally defined function
test1B.C   // no call at all, just a function


specimen01_1.C   // function with return value,  called as part of expression
specimen02_1.C   // function with input parameter
specimen03_1.C   // calling function inside if-condition
specimen04_1.C   // calling a function inside while-condition
specimen05_1.C    // do-while-condition
specimen06_1.C    // for-loop's init statement calls a function
specimen07_1.C   //  for-loop's condition
specimen08_1.C  // for-loop's stop condition
specimen09_1.C  // calling a function inside a variable declaration's initializer
specimen10_1.C  // calling inside a function argument
specimen11_3.C  // nested function calls as arguments
specimen12_3.C  // multiple levels of calls
specimen13_1.C  // three operand expression: op1?op2:op3
specimen14_1.C  // stdlib function exit(1) , system function
specimen15_1.C  // logic expression, similar to above 

specimen16_4.C  // mat and vec norm calculation
* correct_pass16.C // output sample for vec and mat norm calculation, 
specimen17_10.C  // factorial calculation: recursive calls
specimen18_1.C  // foo(), call by reference typed argument
specimen19_1.C  // foo() call by value
specimen20_1.C // w() call with return value, used inside initializer of a declaration
specimen21_1.C // w() again,
specimen22_1.C  // foo() accessing both global and parameter-list scope variables

//----- C++ features 
fail1.C     // C++ constructor, a function return value of object
fail2.C      // C++ constructor , a function returning reference of object

specimen23_1.C // simplest class member function being called
specimen24_1.C  // class member functions return reference types
specimen25_1.C  // template function swap()
specimen26_1.C  // template function GeMax()

testWithTokenStreamUnparsing_01.C //mixed trivial calls and calls in for-loop comparison expression
testWithTokenStreamUnparsing_02.C // even a bit more complex

simplest_raja.C   // simplest raja code
