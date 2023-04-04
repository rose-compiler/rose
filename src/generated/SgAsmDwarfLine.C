#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLine            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfLine::get_address() const {
    return p_address;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLine::set_address(uint64_t const& x) {
    this->p_address = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmDwarfLine::get_file_id() const {
    return p_file_id;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLine::set_file_id(int const& x) {
    this->p_file_id = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmDwarfLine::get_line() const {
    return p_line;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLine::set_line(int const& x) {
    this->p_line = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmDwarfLine::get_column() const {
    return p_column;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLine::set_column(int const& x) {
    this->p_column = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLine::~SgAsmDwarfLine() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLine::SgAsmDwarfLine()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_address(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_file_id(Sg_File_Info::NULL_FILE_ID)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_line(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_column(0) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmDwarfLine
//    property=file_id          class=SgAsmDwarfLine
//    property=line             class=SgAsmDwarfLine
//    property=column           class=SgAsmDwarfLine
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLine::SgAsmDwarfLine(uint64_t const& address,
                               int const& file_id,
                               int const& line,
                               int const& column)
    : p_address(address)
    , p_file_id(file_id)
    , p_line(line)
    , p_column(column) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLine::initializeProperties() {
    p_address = 0;
    p_file_id = Sg_File_Info::NULL_FILE_ID;
    p_line = 0;
    p_column = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
