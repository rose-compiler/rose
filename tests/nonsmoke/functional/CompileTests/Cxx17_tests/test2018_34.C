// Using attribute namespaces without repetition

   [[ using CC: opt(1), debug ]] void f() {}
// Same as [[ CC::opt(1), CC::debug ]] void f() {}

   [[ using CC: opt(1)]][[ CC::debug ]] void g() {}
// Okay (same effect as above).

// [[ using CC: CC::opt(1) ]] void h() {}
// error: cannot combine using and scoped attribute token
