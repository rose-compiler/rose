//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBlock            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBlock_IMPL
#include <sage3basic.h>

rose_addr_t const&
SgAsmBlock::get_id() const {
    return p_id;
}

void
SgAsmBlock::set_id(rose_addr_t const& x) {
    this->p_id = x;
    set_isModified(true);
}

unsigned const&
SgAsmBlock::get_reason() const {
    return p_reason;
}

void
SgAsmBlock::set_reason(unsigned const& x) {
    this->p_reason = x;
    set_isModified(true);
}

SgAsmStatementPtrList const&
SgAsmBlock::get_statementList() const {
    return p_statementList;
}

SgAsmStatementPtrList&
SgAsmBlock::get_statementList() {
    return p_statementList;
}

void
SgAsmBlock::set_statementList(SgAsmStatementPtrList const& x) {
    changeChildPointer(this->p_statementList, const_cast<SgAsmStatementPtrList&>(x));
    set_isModified(true);
}

SgAsmIntegerValuePtrList const&
SgAsmBlock::get_successors() const {
    return p_successors;
}

SgAsmIntegerValuePtrList&
SgAsmBlock::get_successors() {
    return p_successors;
}

void
SgAsmBlock::set_successors(SgAsmIntegerValuePtrList const& x) {
    this->p_successors = x;
    set_isModified(true);
}

bool const&
SgAsmBlock::get_successorsComplete() const {
    return p_successorsComplete;
}

void
SgAsmBlock::set_successorsComplete(bool const& x) {
    this->p_successorsComplete = x;
    set_isModified(true);
}

SgAsmBlock* const&
SgAsmBlock::get_immediateDominator() const {
    return p_immediateDominator;
}

void
SgAsmBlock::set_immediateDominator(SgAsmBlock* const& x) {
    this->p_immediateDominator = x;
    set_isModified(true);
}

size_t const&
SgAsmBlock::get_cachedVertex() const {
    return p_cachedVertex;
}

void
SgAsmBlock::set_cachedVertex(size_t const& x) {
    this->p_cachedVertex = x;
    set_isModified(true);
}

double const&
SgAsmBlock::get_codeLikelihood() const {
    return p_codeLikelihood;
}

void
SgAsmBlock::set_codeLikelihood(double const& x) {
    this->p_codeLikelihood = x;
    set_isModified(true);
}

int64_t const&
SgAsmBlock::get_stackDeltaOut() const {
    return p_stackDeltaOut;
}

void
SgAsmBlock::set_stackDeltaOut(int64_t const& x) {
    this->p_stackDeltaOut = x;
    set_isModified(true);
}

SgAsmBlock::~SgAsmBlock() {
    destructorHelper();
}

SgAsmBlock::SgAsmBlock()
    : p_id(0)
    , p_reason(SgAsmBlock::BLK_NONE)
    , p_successorsComplete(false)
    , p_immediateDominator(nullptr)
    , p_cachedVertex((size_t)(-1))
    , p_codeLikelihood(0.0)
    , p_stackDeltaOut(SgAsmInstruction::INVALID_STACK_DELTA) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
SgAsmBlock::SgAsmBlock(rose_addr_t const& address)
    : SgAsmStatement(address)
    , p_id(0)
    , p_reason(SgAsmBlock::BLK_NONE)
    , p_successorsComplete(false)
    , p_immediateDominator(nullptr)
    , p_cachedVertex((size_t)(-1))
    , p_codeLikelihood(0.0)
    , p_stackDeltaOut(SgAsmInstruction::INVALID_STACK_DELTA) {}

void
SgAsmBlock::initializeProperties() {
    p_id = 0;
    p_reason = SgAsmBlock::BLK_NONE;
    p_successorsComplete = false;
    p_immediateDominator = nullptr;
    p_cachedVertex = (size_t)(-1);
    p_codeLikelihood = 0.0;
    p_stackDeltaOut = SgAsmInstruction::INVALID_STACK_DELTA;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
