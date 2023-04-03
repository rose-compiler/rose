#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLexicalBlock            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfConstructList* const&
SgAsmDwarfLexicalBlock::get_body() const {
    return p_body;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLexicalBlock::set_body(SgAsmDwarfConstructList* const& x) {
    this->p_body = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLexicalBlock::~SgAsmDwarfLexicalBlock() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLexicalBlock::SgAsmDwarfLexicalBlock()
    : p_body(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLexicalBlock::SgAsmDwarfLexicalBlock(int const& nesting_level,
                                               uint64_t const& offset,
                                               uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLexicalBlock::initializeProperties() {
    p_body = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
