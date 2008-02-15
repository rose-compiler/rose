// defining static-member array without array size

// originally found in package fam_2.7.0-6

// a.ii:7:4: error: attempt to create an object of incomplete type `struct S1
// []'

// ERR-MATCH: attempt to create an object of incomplete type

struct S1 {
    static S1 foo[1];
};

S1 S1::foo[];
