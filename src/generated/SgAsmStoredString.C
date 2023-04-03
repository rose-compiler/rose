#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStoredString            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage* const&
SgAsmStoredString::get_storage() const {
    return p_storage;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStoredString::set_storage(SgAsmStringStorage* const& x) {
    this->p_storage = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmStoredString::~SgAsmStoredString() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmStoredString::SgAsmStoredString()
    : p_storage(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStoredString::initializeProperties() {
    p_storage = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
