//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmFunction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

std::string const&
SgAsmFunction::get_name() const {
    return p_name;
}

void
SgAsmFunction::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

unsigned const&
SgAsmFunction::get_reason() const {
    return p_reason;
}

void
SgAsmFunction::set_reason(unsigned const& x) {
    this->p_reason = x;
    set_isModified(true);
}

std::string const&
SgAsmFunction::get_reasonComment() const {
    return p_reasonComment;
}

void
SgAsmFunction::set_reasonComment(std::string const& x) {
    this->p_reasonComment = x;
    set_isModified(true);
}

SgAsmFunction::function_kind_enum const&
SgAsmFunction::get_function_kind() const {
    return p_function_kind;
}

void
SgAsmFunction::set_function_kind(SgAsmFunction::function_kind_enum const& x) {
    this->p_function_kind = x;
    set_isModified(true);
}

SgAsmFunction::MayReturn const&
SgAsmFunction::get_may_return() const {
    return p_may_return;
}

void
SgAsmFunction::set_may_return(SgAsmFunction::MayReturn const& x) {
    this->p_may_return = x;
    set_isModified(true);
}

std::string const&
SgAsmFunction::get_name_md5() const {
    return p_name_md5;
}

void
SgAsmFunction::set_name_md5(std::string const& x) {
    this->p_name_md5 = x;
    set_isModified(true);
}

SgAsmStatementPtrList const&
SgAsmFunction::get_statementList() const {
    return p_statementList;
}

SgAsmStatementPtrList&
SgAsmFunction::get_statementList() {
    return p_statementList;
}

void
SgAsmFunction::set_statementList(SgAsmStatementPtrList const& x) {
    changeChildPointer(this->p_statementList, const_cast<SgAsmStatementPtrList&>(x));
    set_isModified(true);
}

SgAsmStatementPtrList const&
SgAsmFunction::get_dest() const {
    return p_dest;
}

void
SgAsmFunction::set_dest(SgAsmStatementPtrList const& x) {
    this->p_dest = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmFunction::get_entry_va() const {
    return p_entry_va;
}

void
SgAsmFunction::set_entry_va(rose_addr_t const& x) {
    this->p_entry_va = x;
    set_isModified(true);
}

SgSymbolTable* const&
SgAsmFunction::get_symbol_table() const {
    return p_symbol_table;
}

void
SgAsmFunction::set_symbol_table(SgSymbolTable* const& x) {
    this->p_symbol_table = x;
    set_isModified(true);
}

size_t const&
SgAsmFunction::get_cached_vertex() const {
    return p_cached_vertex;
}

void
SgAsmFunction::set_cached_vertex(size_t const& x) {
    this->p_cached_vertex = x;
    set_isModified(true);
}

int64_t const&
SgAsmFunction::get_stackDelta() const {
    return p_stackDelta;
}

void
SgAsmFunction::set_stackDelta(int64_t const& x) {
    this->p_stackDelta = x;
    set_isModified(true);
}

std::string const&
SgAsmFunction::get_callingConvention() const {
    return p_callingConvention;
}

void
SgAsmFunction::set_callingConvention(std::string const& x) {
    this->p_callingConvention = x;
    set_isModified(true);
}

SgAsmFunction::~SgAsmFunction() {
    destructorHelper();
}

SgAsmFunction::SgAsmFunction()
    : p_reason(SgAsmFunction::FUNC_NONE)
    , p_function_kind(SgAsmFunction::e_unknown)
    , p_may_return(SgAsmFunction::RET_UNKNOWN)
    , p_entry_va(0)
    , p_symbol_table(nullptr)
    , p_cached_vertex((size_t)(-1))
    , p_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=name             class=SgAsmFunction
//    property=reason           class=SgAsmFunction
//    property=function_kind    class=SgAsmFunction
SgAsmFunction::SgAsmFunction(rose_addr_t const& address,
                             std::string const& name,
                             unsigned const& reason,
                             SgAsmFunction::function_kind_enum const& function_kind)
    : SgAsmSynthesizedDeclaration(address)
    , p_name(name)
    , p_reason(reason)
    , p_function_kind(function_kind)
    , p_may_return(SgAsmFunction::RET_UNKNOWN)
    , p_entry_va(0)
    , p_symbol_table(nullptr)
    , p_cached_vertex((size_t)(-1))
    , p_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}

void
SgAsmFunction::initializeProperties() {
    p_reason = SgAsmFunction::FUNC_NONE;
    p_function_kind = SgAsmFunction::e_unknown;
    p_may_return = SgAsmFunction::RET_UNKNOWN;
    p_entry_va = 0;
    p_symbol_table = nullptr;
    p_cached_vertex = (size_t)(-1);
    p_stackDelta = SgAsmInstruction::INVALID_STACK_DELTA;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
