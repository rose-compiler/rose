// Object construction improvement
// In C++03, constructors of a class are not allowed to call other constructors of that class; each 
// constructor must construct all of its class members itself or call a common member function, like 
// these,

class SomeType  {
    int number;
 
public:
    SomeType(int new_number) : number(new_number) {}
    SomeType() : number(42) {}
};

class SomeType2  {
    int number;
 
private:
    void Construct(int new_number) { number = new_number; }
public:
    SomeType2(int new_number) { Construct(new_number); }
    SomeType2() { Construct(42); }
};

// Constructors for base classes cannot be directly exposed to derived classes; each derived class must 
// implement constructors even if a base class constructor would be appropriate. Non-constant data members 
// of classes cannot be initialized at the site of the declaration of those members. They can be initialized 
// only in a constructor.

// C++11 provides solutions to all of these problems.

// C++11 allows constructors to call other peer constructors (known as delegation). This allows constructors 
// to utilize another constructor's behavior with a minimum of added code. Examples of other languages similar 
// to C++ that provide delegation are Java, C#, and D.

// This syntax is as follows:

class SomeType3  {
    int number;
 
public:
    SomeType3(int new_number) : number(new_number) {}
    SomeType3() : SomeType3(42) {}
};

// Notice that, in this case, the same effect could have been achieved by making new_number a defaulting parameter. 
// The new syntax, however, allows the default value (42) to be expressed in the implementation rather than the 
// interface \u2014 a benefit to maintainers of library code since default values for function parameters are 
// \u201cbaked in\u201d to call sites, whereas constructor delegation allows the value to be changed without 
// recompilation of the code using the library.

// This comes with a caveat: C++03 considers an object to be constructed when its constructor finishes executing, 
// but C++11 considers an object constructed once any constructor finishes execution. Since multiple constructors 
// will be allowed to execute, this will mean that each delegate constructor will be executing on a fully constructed 
// object of its own type. Derived class constructors will execute after all delegation in their base classes is complete.

// For base-class constructors, C++11 allows a class to specify that base class constructors will be inherited. 
// This means that the C++11 compiler will generate code to perform the inheritance, the forwarding of the derived 
// class to the base class. Note that this is an all-or-nothing feature; either all of that base class's constructors 
// are forwarded or none of them are. Also, note that there are restrictions for multiple inheritance, such that class 
// constructors cannot be inherited from two classes that use constructors with the same signature. Nor can a constructor 
// in the derived class exist that matches a signature in the inherited base class.

// The syntax is as follows:

class BaseClass {
public:
    BaseClass(int value);
};
 
class DerivedClass : public BaseClass {
public:
 // BUG: this is unparsed as "using ::BaseClass;"
    using BaseClass::BaseClass;
};

// For member initialization, C++11 allows the following syntax:

class SomeClass {
public:
    SomeClass() {}
    explicit SomeClass(int new_value) : value(new_value) {}
 
private:
    int value = 5;
};

// Any constructor of the class will initialize value with 5, if the constructor does not override the initialization 
// with its own. So the above empty constructor will initialize value as the class definition states, but the constructor 
// that takes an int will initialize it to the given parameter.

// It can also use constructor or uniform initialization, instead of the equality initialization shown above.
