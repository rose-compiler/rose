// Problem reported by Yarden

class A {};
class B {
 // static A a;  This works fine!
    static const A a;

 // Generates "B(class A a1= *((class A *)(&B::a)));"
    B(A a1 = a);
};

// DQ: Added this to support the complete static initialization (not required to demonstrate bug)
//     In fact if this is here it is an error in the code generated (g++ reports it as:
//     "error: uninitialized const B::a").
// const A B::a;
// A B::a;  This works fine!
