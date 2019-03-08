// This test code demostrates the language concept of "inheriting constructors"

class BaseClass {
public:
    BaseClass(int value);
};
 
class DerivedClass : public BaseClass {
public:
 // BUG: this is unparsed as "using ::BaseClass;"
    using BaseClass::BaseClass;
};

