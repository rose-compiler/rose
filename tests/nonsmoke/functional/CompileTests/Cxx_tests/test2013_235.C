// Note that this fails:
//    ../../testGraphGeneration -isystem /home/dquinlan/local/boost_1_42_0_installTree-gxx-4.2.4/include -c /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/nonsmoke/functional/CompileTests/RoseExample_tests/testRoseHeaders_07.C
// where as this succeeds:
//    ../../testGraphGeneration -c /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/nonsmoke/functional/CompileTests/RoseExample_tests/testRoseHeaders_07.C
// 
// The reason is that ROSE works when the isystem option is not specified is that EDG is seeing 
// BOOST version 1.33 in /usr/include when we don't use the -isystem options but is seeing BOOST 
// version 1.42 when compiling with the -isystem option. When using the -isystem option, ROSE 
// converts this to the --sys_include option for EDG.
//
// ROSE currently has a bug that prevents it from handling BOOST 1.42 for this specific code,
// but ROSE handles BOOST 1.33 (likely because version 1.33 is different code than version 1.42).

// DQ (1/27/2014): Commented out for now so that we can get the C support out.
// This only fails with the 4.4.5 compiler when optimizied.  Unoptimized it
// runs fine and with the 4.2.4 compiler it compiles fine as well (optimized 
// and unoptimized).  I have to look into this in more depth next week.  Then
// I will turn it back on.  We still have other Boost specific tests.
#if 1
#include <boost/graph/adjacency_list.hpp>
#endif
