//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLine            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfLine_IMPL
#include <sage3basic.h>

uint64_t const&
SgAsmDwarfLine::get_address() const {
    return p_address;
}

void
SgAsmDwarfLine::set_address(uint64_t const& x) {
    this->p_address = x;
    set_isModified(true);
}

int const&
SgAsmDwarfLine::get_file_id() const {
    return p_file_id;
}

void
SgAsmDwarfLine::set_file_id(int const& x) {
    this->p_file_id = x;
    set_isModified(true);
}

int const&
SgAsmDwarfLine::get_line() const {
    return p_line;
}

void
SgAsmDwarfLine::set_line(int const& x) {
    this->p_line = x;
    set_isModified(true);
}

int const&
SgAsmDwarfLine::get_column() const {
    return p_column;
}

void
SgAsmDwarfLine::set_column(int const& x) {
    this->p_column = x;
    set_isModified(true);
}

SgAsmDwarfLine::~SgAsmDwarfLine() {
    destructorHelper();
}

SgAsmDwarfLine::SgAsmDwarfLine()
    : p_address(0)
    , p_file_id(-2)
    , p_line(0)
    , p_column(0) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmDwarfLine
//    property=file_id          class=SgAsmDwarfLine
//    property=line             class=SgAsmDwarfLine
//    property=column           class=SgAsmDwarfLine
SgAsmDwarfLine::SgAsmDwarfLine(uint64_t const& address,
                               int const& file_id,
                               int const& line,
                               int const& column)
    : p_address(address)
    , p_file_id(file_id)
    , p_line(line)
    , p_column(column) {}

void
SgAsmDwarfLine::initializeProperties() {
    p_address = 0;
    p_file_id = -2;
    p_line = 0;
    p_column = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
