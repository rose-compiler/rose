//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBlock            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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
    this->p_statementList = x;
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
SgAsmBlock::get_successors_complete() const {
    return p_successors_complete;
}

void
SgAsmBlock::set_successors_complete(bool const& x) {
    this->p_successors_complete = x;
    set_isModified(true);
}

SgAsmBlock* const&
SgAsmBlock::get_immediate_dominator() const {
    return p_immediate_dominator;
}

void
SgAsmBlock::set_immediate_dominator(SgAsmBlock* const& x) {
    this->p_immediate_dominator = x;
    set_isModified(true);
}

size_t const&
SgAsmBlock::get_cached_vertex() const {
    return p_cached_vertex;
}

void
SgAsmBlock::set_cached_vertex(size_t const& x) {
    this->p_cached_vertex = x;
    set_isModified(true);
}

double const&
SgAsmBlock::get_code_likelihood() const {
    return p_code_likelihood;
}

void
SgAsmBlock::set_code_likelihood(double const& x) {
    this->p_code_likelihood = x;
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
    , p_successors_complete(false)
    , p_immediate_dominator(nullptr)
    , p_cached_vertex((size_t)(-1))
    , p_code_likelihood(0.0)
    , p_stackDeltaOut(SgAsmInstruction::INVALID_STACK_DELTA) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
SgAsmBlock::SgAsmBlock(rose_addr_t const& address)
    : SgAsmStatement(address) {}

void
SgAsmBlock::initializeProperties() {
    p_id = 0;
    p_reason = SgAsmBlock::BLK_NONE;
    p_successors_complete = false;
    p_immediate_dominator = nullptr;
    p_cached_vertex = (size_t)(-1);
    p_code_likelihood = 0.0;
    p_stackDeltaOut = SgAsmInstruction::INVALID_STACK_DELTA;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
