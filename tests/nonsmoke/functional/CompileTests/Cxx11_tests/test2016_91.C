// Subset that previously worked in EDG.
class alignas(1) DepGraphNode1 {};
class alignas(2) DepGraphNode2 {};
class alignas(4) DepGraphNode4 {};
class alignas(8) DepGraphNode8 {};
class alignas(16) DepGraphNode16 {};
class alignas(32) DepGraphNode32 {};
class alignas(64) DepGraphNode64 {};
class alignas(128) DepGraphNode128 {};

// Newly added range of alignment attributes added to EDG (required for RAJA code).
class alignas(256) DepGraphNode256 {};

#if ( (__GNUC__ == 6) && (__GNUC_MINOR__ >= 1) )
// Initially, let's not expect this to pass on the full range of compilers where we test.
class alignas(512) DepGraphNode512 {};
class alignas(1024) DepGraphNode1024 {};
class alignas(2048) DepGraphNode2048 {};
class alignas(4096) DepGraphNode4096 {};
class alignas(8192) DepGraphNode8192 {};
class alignas(16384) DepGraphNode16384 {};
#endif
