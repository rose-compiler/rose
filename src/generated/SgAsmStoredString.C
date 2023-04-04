#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStoredString            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage* const&
SgAsmStoredString::get_storage() const {
    return p_storage;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStoredString::set_storage(SgAsmStringStorage* const& x) {
    this->p_storage = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmStoredString::~SgAsmStoredString() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmStoredString::SgAsmStoredString()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_storage(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStoredString::initializeProperties() {
    p_storage = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
