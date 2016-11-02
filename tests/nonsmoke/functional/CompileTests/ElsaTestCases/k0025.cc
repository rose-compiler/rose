// fully-namespace-qualified base class initialization

// originally found in package centericq

// ERR-MATCH: Parse error.* at ::$

namespace N {
    struct S1 {
        S1() {}
    };

    struct S2 : public ::N::S1 {
        S2() : ::N::S1() {
        };
    };

    struct T1 {
        struct S3 : public ::N::S1 {
            S3() : ::N::S1() {
            };
        };
    };

    // even if S1 is the same name, but in an enclosing scope
    struct T2 {
        struct S1 : public ::N::S1 {
            S1() : ::N::S1() {
            };
        };
    };

}
