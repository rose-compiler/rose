// This appears to be a bug in EDG 4.7, when EDG is configured to be compiled 
// to emulate GNU 4.4 compiler, using:
//    --gnu_version 40405 -D__GNUG__=4 -D__GNUC__=4 -D__GNUC_MINOR__=4 -D__GNUC_PATCHLEVEL__=5

// Note that this is extracted from a header file for our ROSE project and thus is a
// part of every ROSE translator.  We detect this as part of tests for our ROSE compiler
// project (a C++ application) to compile itself.

#include <boost/graph/adjacency_list.hpp>

class SgAsmBlock;

namespace BinaryAnalysis 
   {
     class ControlFlow 
        {
          public:
               typedef boost::adjacency_list<boost::setS,                                  /* edges of each vertex in std::list */
                                             boost::vecS,                                  /* vertices in std::vector */
                                             boost::bidirectionalS,
                                             boost::property<boost::vertex_name_t, SgAsmBlock*> > Graph;
        };
   }

class AsmUnparser 
   {
     public:
         typedef BinaryAnalysis::ControlFlow::Graph CFG;

      // DQ (7/3/2013): This code will compile fine with ROSE configured to be a GNU 4.4.5 compiler if this line is commented out.
      // Uncommented the file will compile only if ROSE is configured to be a GNU 4.2.4 compiler.
      // For both GNU g++ version 4.2.4 and version 4.4.5 the file compiles just fine (with the line uncommented).
#if 1
         typedef boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;
#endif
   };

