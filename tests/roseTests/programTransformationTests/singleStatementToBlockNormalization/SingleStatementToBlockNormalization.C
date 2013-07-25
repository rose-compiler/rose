#include "rose.h"
#include <SingleStatementToBlockNormalization.h>


int main(int argc, char** argv) {
    SgProject* project = frontend(argc, argv);
    ROSE_ASSERT(project != NULL);
    AstTests::runAllTests(project);
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(project);
    AstTests::runAllTests(project);
    return backend(project);
}
