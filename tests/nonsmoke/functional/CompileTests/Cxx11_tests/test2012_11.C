// Range-based for-loop
// In C++03, iterating over the elements of a list requires a lot of code. Other languages have implemented 
// support for syntactic sugar that allow the programmer to write a simple \u201cforeach\u201d statement that 
// automatically traverses items in a list. One of those languages is the Java programming language, which 
// received support for what has been defined as enhanced for loops in Java 5.0.[9]

// C++11 added a similar feature. The statement for allows for easy iteration over a list of elements:

int my_array[5] = {1, 2, 3, 4, 5};

void foobar()
   {
     for (int &x : my_array)
        {
          x *= 2;
        }
   }

// This form of for, called the \u201crange-based for\u201d, will iterate over each element in the list. It 
// will work for C-style arrays, initializer lists, and any type that has begin() and end() functions defined 
// for it that return iterators. All of the standard library containers that have begin/end pairs will work 
// with the range-based for statement.
