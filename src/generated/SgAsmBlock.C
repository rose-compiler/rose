#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBlock            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmBlock::get_id() const {
    return p_id;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_id(rose_addr_t const& x) {
    this->p_id = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmBlock::get_reason() const {
    return p_reason;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_reason(unsigned const& x) {
    this->p_reason = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmStatementPtrList const&
SgAsmBlock::get_statementList() const {
    return p_statementList;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmStatementPtrList&
SgAsmBlock::get_statementList() {
    return p_statementList;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_statementList(SgAsmStatementPtrList const& x) {
    this->p_statementList = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmIntegerValuePtrList const&
SgAsmBlock::get_successors() const {
    return p_successors;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmIntegerValuePtrList&
SgAsmBlock::get_successors() {
    return p_successors;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_successors(SgAsmIntegerValuePtrList const& x) {
    this->p_successors = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmBlock::get_successors_complete() const {
    return p_successors_complete;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_successors_complete(bool const& x) {
    this->p_successors_complete = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmBlock* const&
SgAsmBlock::get_immediate_dominator() const {
    return p_immediate_dominator;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_immediate_dominator(SgAsmBlock* const& x) {
    this->p_immediate_dominator = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmBlock::get_cached_vertex() const {
    return p_cached_vertex;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_cached_vertex(size_t const& x) {
    this->p_cached_vertex = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
double const&
SgAsmBlock::get_code_likelihood() const {
    return p_code_likelihood;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_code_likelihood(double const& x) {
    this->p_code_likelihood = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int64_t const&
SgAsmBlock::get_stackDeltaOut() const {
    return p_stackDeltaOut;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBlock::set_stackDeltaOut(int64_t const& x) {
    this->p_stackDeltaOut = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmBlock::~SgAsmBlock() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmBlock::SgAsmBlock()
    : p_id(0)
    , p_reason(SgAsmBlock::BLK_NONE)
    , p_successors_complete(false)
    , p_immediate_dominator(nullptr)
    , p_cached_vertex((size_t)(-1))
    , p_code_likelihood(0.0)
    , p_stackDeltaOut(SgAsmInstruction::INVALID_STACK_DELTA) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmBlock::SgAsmBlock(rose_addr_t const& address)
    : SgAsmStatement(address) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
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

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
