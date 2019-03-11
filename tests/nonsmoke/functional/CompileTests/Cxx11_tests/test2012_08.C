// Example from C++11 Wiki web page:

#include<initializer_list>
#include<vector>
#include<string>

// Initializer lists
// C++03 inherited the initializer-list feature from C. A struct or array is given a list of 
// arguments in curly brackets, in the order of the members' definitions in the struct. These 
// initializer-lists are recursive, so an array of structs or struct containing other structs 
// can use them.

struct Object
{
    float first;
    int second;
};
 
Object scalar = {0.43f, 10}; //One Object, with first=0.43f and second=10
Object anArray[] = {{13.4f, 3}, {43.28f, 29}, {5.934f, 17}}; //An array of three Objects

// This is very useful for static lists or just for initializing a struct to a particular value. 
// C++ also provides constructors to initialize an object, but they are often not as convenient 
// as the initializer list. However C++03 allows initializer-lists only on structs and classes 
// that conform to the Plain Old Data (POD) definition; C++11 extends initializer-lists, so they 
// can be used for all classes including standard containers like std::vector.

// C++11 binds the concept to a template, called std::initializer_list. This allows constructors 
// and other functions to take initializer-lists as parameters. For example:

class SequenceClass {
public:
    SequenceClass(std::initializer_list<int> list);
};

// This allows SequenceClass to be constructed from a sequence of integers, as such:

SequenceClass some_var = {1, 4, 5, 6};

// This constructor is a special kind of constructor, called an initializer-list-constructor. 
// Classes with such a constructor are treated specially during uniform initialization (see below)

// The class std::initializer_list<> is a first-class C++11 standard library type. However, they can
// be initially constructed statically by the C++11 compiler only through the use of the {} syntax. 
// The list can be copied once constructed, though this is only a copy-by-reference. An initializer 
// list is constant; its members cannot be changed once the initializer list is created, nor can 
// the data in those members be changed.

// Because initializer_list is a real type, it can be used in other places besides class constructors. 
// Regular functions can take typed initializer lists as arguments. For example:

void function_name(std::initializer_list<float> list);

void foobar()
   {
  // function_name({1.0f, -3.45f, -0.4f});
     function_name({1.0f, -3.45f, -0.4f});
   }

// Standard containers can also be initialized in the following ways:

std::vector<std::string> v1 = { "xyzzy", "plugh", "abracadabra" };
std::vector<std::string> v2 ({ "xyzzy", "plugh", "abracadabra" });
std::vector<std::string> v3 { "xyzzy", "plugh", "abracadabra" }; // see "Uniform initialization" below

