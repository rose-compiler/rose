#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilDataStream            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilDataStream::get_Offset() const {
    return p_Offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilDataStream::set_Offset(uint32_t const& x) {
    this->p_Offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilDataStream::get_Size() const {
    return p_Size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilDataStream::set_Size(uint32_t const& x) {
    this->p_Size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmCilDataStream::get_Name() const {
    return p_Name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilDataStream::set_Name(std::string const& x) {
    this->p_Name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilDataStream::get_NamePadding() const {
    return p_NamePadding;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilDataStream::set_NamePadding(uint32_t const& x) {
    this->p_NamePadding = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCilDataStream::~SgAsmCilDataStream() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCilDataStream::SgAsmCilDataStream() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmCilDataStream::SgAsmCilDataStream(uint32_t const& Offset,
                                       uint32_t const& Size,
                                       std::string const& Name,
                                       uint32_t const& NamePadding)
    : p_Offset(Offset)
    , p_Size(Size)
    , p_Name(Name)
    , p_NamePadding(NamePadding) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilDataStream::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
