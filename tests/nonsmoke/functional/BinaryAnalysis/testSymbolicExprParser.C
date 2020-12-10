#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <BinarySymbolicExprParser.h>

using namespace Rose::BinaryAnalysis;

static const char *inputs[] = {
    // Numeric constants
    "123",                                              // 0x0000007b[32]
    "0x0000007b[32]",
    "0x0000007b[u32]",
    "-123",                                             // 0xffffff85<4294967173,-123>[32]
    "0xffffff85<4294967173,-123>[32]",
    "0xffffff85<4294967173,-123>[u32]",
    "123[4]",                                           // 0xb<-5>[4]
    "123[u4]",
    "0xb<-5>[4]",
    "0xb<-5>[u4]",
    "-5[4]",
    "-5[u4]",
    "0x123456789abcdef00fedcba987654321[256]",          // 0x000....00123456789abcdef00fedcba987654321[256]
    "0x123456789abcdef00fedcba987654321[u256]",

    // Variables
    "v1[8]",                                            // v1[8]
    "v1[u8]",
    "v1[f32]",
    "v1[f64]",
    "v1<this is a comment>[8]",
    "v1<this is a comment>[u8]",
    "v100[32]",
    "v100[u32]",

    // Single-level expressions
    "(add v1[32] v2[32])",                                // (add[32] a[32] b[32])
    "(add v1[u32] v2[u32])",
    "(add[32] v1[32] v2[32])",
    "(add[u32] v1[u32] v2[u32])",
    "(add 10 20)",                                      // (add[32] 0x0000001e[32])

    // Nested expressions
    "(negate (negate 1))",                              // 0x00000001[32]
    "(add (umul v1[8] v2[8]) (umul v3[8] v4[8]))",
    "(add (umul v1[u8] v2[u8]) (umul v3[u8] v4[u8]))"
};

int
main() {
    SymbolicExprParser parser;
    for (size_t i=0; i<sizeof(inputs)/sizeof(*inputs); ++i) {
        std::cout <<"Parser input: " <<inputs[i] <<"\n";
        SymbolicExpr::Ptr symbolicExpression = parser.parse(inputs[i]);
        std::cout <<"Parse expr:   " <<*symbolicExpression <<"\n\n";
        
    }
}

#endif
