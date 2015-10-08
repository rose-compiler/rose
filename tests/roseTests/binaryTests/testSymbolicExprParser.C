#include <rose.h>

#include <InsnSemanticsExpr.h>
#include <BinarySymbolicExprParser.h>

using namespace rose::BinaryAnalysis;

static const char *inputs[] = {
    // Numeric constants
    "123",                                              // 0x0000007b[32]
    "0x0000007b[32]",
    "-123",                                             // 0xffffff85<4294967173,-123>[32]
    "0xffffff85<4294967173,-123>[32]",
    "123[4]",                                           // 0xb<-5>[4]
    "0xb<-5>[4]",
    "-5[4]",
    "0x123456789abcdef00fedcba987654321[256]",          // 0x000....00123456789abcdef00fedcba987654321[256]

    // Variables
    "foo[8]",                                           // foo[8]
    "foo<this is a comment>[8]",
    "v100[32]",

    // Single-level expressions
    "(add a[32] b[32])",                                // (add[32] a[32] b[32])
    "(add[32] a[32] b[32])",
    "(add 10 20)",                                      // (add[32] 0x0000001e[32])

    // Nested expressions
    "(negate (negate 1))",                              // 0x00000001[32]
    "(add (umul a[8] b[8]) (umul c[8] d[8]))"
};

int
main() {
    SymbolicExprParser parser;
    for (size_t i=0; i<sizeof(inputs)/sizeof(*inputs); ++i) {
        std::cout <<"Parser input: " <<inputs[i] <<"\n";
        InsnSemanticsExpr::TreeNodePtr symbolicExpression = parser.parse(inputs[i]);
        std::cout <<"Parse expr:   " <<*symbolicExpression <<"\n\n";
        
    }
}
