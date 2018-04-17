// constructor member initialization without namespace qualifier

// originally found in package omniorb4_4.0.5-1

// a.ii:9:5: error: `S1' does not denote any class

// ERR-MATCH: `.*?' does not denote any class

namespace NS {
    struct B {
    };
}

struct D : NS::B {
    D() : B() {
    }
};
