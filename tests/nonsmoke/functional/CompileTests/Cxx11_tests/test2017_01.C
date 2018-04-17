struct Struct1 {
    enum class Enum1 {
        EVal1
    };

    Enum1 member1;

    Struct1() : member1(Enum1::EVal1) {}
 // Struct1() : member1(0) {} // this fails since type promotions is not allowed for this construct.

// Does not fail:
//  void method1() {
//      member1 = Enum1::EVal1; }

};
