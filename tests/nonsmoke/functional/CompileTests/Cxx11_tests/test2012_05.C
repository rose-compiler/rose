// Rvalue references and move constructors

// In C++03 (and before), temporaries (termed "rvalues", as they often lie on the right side of an assignment) 
// were intended to never be modifiable \u2014 just as in C \u2014 and were considered to be indistinguishable 
// from const T& types; nevertheless, in some cases, temporaries could have been modified, a behavior that was 
// even considered to be a useful loophole (for the former, see [7]). C++11 adds a new non-const reference type 
// called an rvalue reference, identified by T&&. This refers to temporaries that are permitted to be modified 
// after they are initialized, for the purpose of allowing "move semantics".

// A chronic performance problem with C++03 is the costly and unnecessary deep copies that can happen implicitly 
// when objects are passed by value. To illustrate the issue, consider that a std::vector<T> is, internally, a 
// wrapper around a C-style array with a size. If a std::vector<T> temporary is created or returned from a function, 
// it can be stored only by creating a new std::vector<T> and copying all of the rvalue's data into it. Then the 
// temporary and all its memory is destroyed. (For simplicity, this discussion neglects the return value optimization).

// In C++11, a "move constructor" of std::vector<T> that takes an rvalue reference to a std::vector<T> can copy 
// the pointer to the internal C-style array out of the rvalue into the new std::vector<T>, then set the pointer 
// inside the rvalue to null. Since the temporary will never again be used, no code will try to access the null 
// pointer, and because the pointer is null, its memory is not deleted when it goes out of scope. Hence, the 
// operation not only forgoes the expense of a deep copy, but is safe and invisible.

// Rvalue references can provide performance benefits to existing code without needing to make any changes outside 
// the standard library. The type of the returned value of a function returning a std::vector<T> temporary does 
// not need to be changed explicitly to std::vector<T> && to invoke the move constructor, as temporaries are 
// considered rvalues automatically. (However, if std::vector<T> is a C++03 version without a move constructor, 
// then the copy constructor will be invoked with a const std::vector<T>& as normal, incurring a significant 
// memory allocation.)

// For safety reasons, some restrictions are imposed. A named variable will never be considered to be an rvalue 
// even if it is declared as such; in order to get an rvalue, the function template std::move<T>() should be used. 
// Rvalue references can also be modified only under certain circumstances, being intended to be used primarily 
// with move constructors.

// Due to the nature of the wording of rvalue references, and to some modification to the wording for lvalue 
// references (regular references), rvalue references allow developers to provide perfect function forwarding. 
// When combined with variadic templates, this ability allows for function templates that can perfectly forward 
// arguments to another function that takes those particular arguments. This is most useful for forwarding 
// constructor parameters, to create factory functions that will automatically call the correct constructor for 
// those particular arguments. This is seen in the emplace_back set of STL methods.

// #error "NEED AND EXAMPLE OF THIS!"

typedef unsigned long size_t;

class MemoryPage
   {
     size_t size;
     char*  buf;

     public:
          explicit MemoryPage(int sz=512) : size(sz), buf(new char [size]) {}
         ~MemoryPage() { delete[] buf;}
       // typical C++03 copy ctor and assignment operator
          MemoryPage(const MemoryPage&);
          MemoryPage& operator=(const MemoryPage&);

          MemoryPage(MemoryPage&& other);

   };

// A typical move constructor definition would look like this:

// C++11
MemoryPage::MemoryPage(MemoryPage&& other) : size(0) // , buf(nullptr)
   {
  // pilfer other’s resource
     size = other.size;
     buf = other.buf;
  // reset other
     other.size = 0;
//   other.buf = nullptr;
   }
