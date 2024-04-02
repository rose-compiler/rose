//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmFunction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmFunction_IMPL
#include <SgAsmFunction.h>

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
SgAsmFunction::get_functionKind() const {
    return p_functionKind;
}

void
SgAsmFunction::set_functionKind(SgAsmFunction::function_kind_enum const& x) {
    this->p_functionKind = x;
    set_isModified(true);
}

SgAsmFunction::MayReturn const&
SgAsmFunction::get_mayReturn() const {
    return p_mayReturn;
}

void
SgAsmFunction::set_mayReturn(SgAsmFunction::MayReturn const& x) {
    this->p_mayReturn = x;
    set_isModified(true);
}

std::string const&
SgAsmFunction::get_nameMd5() const {
    return p_nameMd5;
}

void
SgAsmFunction::set_nameMd5(std::string const& x) {
    this->p_nameMd5 = x;
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
SgAsmFunction::get_entryVa() const {
    return p_entryVa;
}

void
SgAsmFunction::set_entryVa(rose_addr_t const& x) {
    this->p_entryVa = x;
    set_isModified(true);
}

SgSymbolTable* const&
SgAsmFunction::get_symbolTable() const {
    return p_symbolTable;
}

void
SgAsmFunction::set_symbolTable(SgSymbolTable* const& x) {
    this->p_symbolTable = x;
    set_isModified(true);
}

size_t const&
SgAsmFunction::get_cachedVertex() const {
    return p_cachedVertex;
}

void
SgAsmFunction::set_cachedVertex(size_t const& x) {
    this->p_cachedVertex = x;
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
    , p_functionKind(SgAsmFunction::e_unknown)
    , p_mayReturn(SgAsmFunction::RET_UNKNOWN)
    , p_entryVa(0)
    , p_symbolTable(nullptr)
    , p_cachedVertex((size_t)(-1))
    , p_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=name             class=SgAsmFunction
//    property=reason           class=SgAsmFunction
//    property=functionKind     class=SgAsmFunction
SgAsmFunction::SgAsmFunction(rose_addr_t const& address,
                             std::string const& name,
                             unsigned const& reason,
                             SgAsmFunction::function_kind_enum const& functionKind)
    : SgAsmSynthesizedDeclaration(address)
    , p_name(name)
    , p_reason(reason)
    , p_functionKind(functionKind)
    , p_mayReturn(SgAsmFunction::RET_UNKNOWN)
    , p_entryVa(0)
    , p_symbolTable(nullptr)
    , p_cachedVertex((size_t)(-1))
    , p_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}

void
SgAsmFunction::initializeProperties() {
    p_reason = SgAsmFunction::FUNC_NONE;
    p_functionKind = SgAsmFunction::e_unknown;
    p_mayReturn = SgAsmFunction::RET_UNKNOWN;
    p_entryVa = 0;
    p_symbolTable = nullptr;
    p_cachedVertex = (size_t)(-1);
    p_stackDelta = SgAsmInstruction::INVALID_STACK_DELTA;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
