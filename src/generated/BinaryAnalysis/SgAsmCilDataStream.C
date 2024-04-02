//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilDataStream            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilDataStream_IMPL
#include <SgAsmCilDataStream.h>

uint32_t const&
SgAsmCilDataStream::get_Offset() const {
    return p_Offset;
}

void
SgAsmCilDataStream::set_Offset(uint32_t const& x) {
    this->p_Offset = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilDataStream::get_Size() const {
    return p_Size;
}

void
SgAsmCilDataStream::set_Size(uint32_t const& x) {
    this->p_Size = x;
    set_isModified(true);
}

std::string const&
SgAsmCilDataStream::get_Name() const {
    return p_Name;
}

void
SgAsmCilDataStream::set_Name(std::string const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilDataStream::get_NamePadding() const {
    return p_NamePadding;
}

void
SgAsmCilDataStream::set_NamePadding(uint32_t const& x) {
    this->p_NamePadding = x;
    set_isModified(true);
}

SgAsmCilDataStream::~SgAsmCilDataStream() {
    destructorHelper();
}

SgAsmCilDataStream::SgAsmCilDataStream() {}

// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
SgAsmCilDataStream::SgAsmCilDataStream(uint32_t const& Offset,
                                       uint32_t const& Size,
                                       std::string const& Name,
                                       uint32_t const& NamePadding)
    : p_Offset(Offset)
    , p_Size(Size)
    , p_Name(Name)
    , p_NamePadding(NamePadding) {}

void
SgAsmCilDataStream::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
