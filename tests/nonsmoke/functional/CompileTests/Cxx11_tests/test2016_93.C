// Max extend of alignment without extension implemented in EDG 4.12 (test of code from Raja).
class DepGraphNode128
{
} __attribute__((aligned(128)));

// DQ (12/10/2016): We only want to test this when using EDG 4.12 and later 
// (since ROSE did not extend this support in the earlier EDG versions).
// Newly added range of alignment attributes added to EDG (required for RAJA code).
#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 412
class DepGraphNode256 
{
} __attribute__((aligned(256)));
#endif
