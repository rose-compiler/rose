static const char *purpose = "check that interpretation's register dictionary is set";
static const char *description =
    "Test that calls Rose::BinaryAnalysis::Partitioner2::Engine::frontend and then checks that the "
    "return value has a SgAsmInterpretation parent whose registers property is non-null.";

#include <rose.h>                                       // must be first
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    SgAsmBlock *gblock = Rose::BinaryAnalysis::Partitioner2::Engine().frontend(argc, argv, purpose, description);
    ASSERT_always_not_null2(gblock, "Engine::frontend didn't return a SgAsmBlock pointer");

    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(gblock);
    ASSERT_always_not_null2(interp, "The gblock doesn't have SgAsmInterpretation as an ancestor");

    Rose::BinaryAnalysis::RegisterDictionary::Ptr registerDictionary = interp->get_registers();
    ASSERT_always_not_null2(registerDictionary, "The SgAsmInterpretation::registers property should not be null");
}
