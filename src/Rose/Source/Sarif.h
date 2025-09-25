#ifndef ROSE_Source_Sarif_H
#define ROSE_Source_Sarif_H

#include <RoseFirst.h>
#include <Rose/Sarif.h>

class SgLocatedNode;

namespace Rose {
namespace Source {
//! Collection of functions to generate SARIF reports based on the AST of source-code
namespace Sarif {

using namespace Rose::Sarif;

LocationPtr locate(SgLocatedNode * node);

}
}
}

#endif /* ROSE_Source_Sarif_H */
