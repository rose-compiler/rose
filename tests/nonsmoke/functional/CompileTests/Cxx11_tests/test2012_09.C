// Example from C++11 Wiki web page:

#include<vector>
#include<string>

// Uniform initialization
// C++03 has a number of problems with initializing types. There are several ways to initialize types, and they 
// do not all produce the same results when interchanged. The traditional constructor syntax, for example, can 
// look like a function declaration, and steps must be taken to ensure that the compiler's most vexing parse 
// rule will not mistake it for such. Only aggregates and POD types can be initialized with aggregate initializers
// (using SomeType var = {/*stuff*/};).

// C++11 provides a syntax that allows for fully uniform type initialization that works on any object. It expands 
// on the initializer list syntax:

struct BasicStruct {
    int x;
    double y;
};
 
struct AltStruct {
    AltStruct(int x, double y) : x_{x}, y_{y} {}
 
private:
    int x_;
    double y_;
};
 
BasicStruct var1{5, 3.2};
AltStruct var2{2, 4.3};

// The initialization of var1 behaves exactly as though it were aggregate-initialization. That is, each data member 
// of an object, in turn, will be copy-initialized with the corresponding value from the initializer-list. Implicit 
// type conversion will be used where necessary. If no conversion exists, or only a narrowing conversion exists, 
// the program is ill-formed. The initialization of var2 invokes the constructor.

// One is also able to do the following:

struct IdString {
    std::string name;
    int identifier;
};
 
IdString get_string()
{
    return {"foo", 42}; //Note the lack of explicit type.
}

// Uniform initialization does not replace constructor syntax. There are still times when constructor syntax is required. 
// If a class has an initializer list constructor (TypeName(initializer_list<SomeType>);), then it takes priority over 
// other forms of construction, provided that the initializer list conforms to the sequence constructor's type. The C++11 
// version of std::vector has an initializer list constructor for its template type. This means that the following code:

std::vector<int> the_vec{4};

// will call the initializer list constructor, not the constructor of std::vector that takes a single size parameter and 
// creates the vector with that size. To access the latter constructor, the user will need to use the standard constructor 
// syntax directly.
