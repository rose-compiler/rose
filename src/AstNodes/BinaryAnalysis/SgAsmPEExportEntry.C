#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

using namespace Rose;

Sawyer::Result<unsigned, std::string>
SgAsmPEExportEntry::biasedOrdinal() const {
    if (auto exportSection = SageInterface::getEnclosingNode<SgAsmPEExportSection>(this)) {
        if (auto exportDir = exportSection->get_exportDirectory()) {
            return Sawyer::makeOk(get_ordinal() + exportDir->get_ord_base());
        } else {
            return Sawyer::makeError("export section has no export directory");
        }
    } else {
        return Sawyer::makeError("export entry is not contained in an export section");
    }
}

#endif
