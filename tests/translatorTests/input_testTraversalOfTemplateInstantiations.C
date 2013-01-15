// This code demonstrates a bug (the AST traversal does not visit the template instantiation).
// It works fine with the instantaited templat is not in the namespace, but fails using the namespace.

#define DEMO_BUG 1

#if DEMO_BUG
namespace NS {
#endif
    template<typename T>
    void
    mm() {
        return;
    }
#if DEMO_BUG
 }
#endif

void foo() {
#if DEMO_BUG
    NS::mm<int>();
#else
    mm<int>();
#endif
}
