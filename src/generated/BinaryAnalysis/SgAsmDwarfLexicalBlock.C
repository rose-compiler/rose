//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLexicalBlock            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfLexicalBlock_IMPL
#include <SgAsmDwarfLexicalBlock.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfLexicalBlock::get_body() const {
    return p_body;
}

void
SgAsmDwarfLexicalBlock::set_body(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_body, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

SgAsmDwarfLexicalBlock::~SgAsmDwarfLexicalBlock() {
    destructorHelper();
}

SgAsmDwarfLexicalBlock::SgAsmDwarfLexicalBlock()
    : p_body(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfLexicalBlock::SgAsmDwarfLexicalBlock(int const& nesting_level,
                                               uint64_t const& offset,
                                               uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(nullptr) {}

void
SgAsmDwarfLexicalBlock::initializeProperties() {
    p_body = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
