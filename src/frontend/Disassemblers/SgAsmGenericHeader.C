#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <SgAsmGenericHeader.h>
#include <SgAsmGenericDLLList.h>

#include <vector>

std::vector<SgAsmGenericDLL*>&
SgAsmGenericHeader::get_dlls() {
    ASSERT_not_null(p_dlls);
    return p_dlls->get_dlls();
}

#endif
