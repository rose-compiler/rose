// ROSE-723

struct Struct1 {
    enum class Enum1 {
        EVal1
    };
    Struct1() :
            member1(Enum1::EVal1) {}
// Does not fail:
//    void method1() {
//        member1 = Enum1::EVal1; }
    Enum1 member1;
};
