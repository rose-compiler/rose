#ifndef ROSE_SourceCode_Sarif_H
#define ROSE_SourceCode_Sarif_H

#include <RoseFirst.h>
#include <Rose/Sarif.h>

class SgLocatedNode;

namespace Rose {
namespace SourceCode {
//! Collection of functions to generate SARIF reports based on the AST of source-code
namespace Sarif {

using namespace Rose::Sarif;

LocationPtr locate(SgLocatedNode * node);

}
}
}

#endif /* ROSE_SourceCode_Sarif_H */
