//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDOSExtendedHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDOSExtendedHeader_IMPL
#include <SgAsmDOSExtendedHeader.h>

unsigned const&
SgAsmDOSExtendedHeader::get_e_res1() const {
    return p_e_res1;
}

void
SgAsmDOSExtendedHeader::set_e_res1(unsigned const& x) {
    this->p_e_res1 = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_oemid() const {
    return p_e_oemid;
}

void
SgAsmDOSExtendedHeader::set_e_oemid(unsigned const& x) {
    this->p_e_oemid = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_oeminfo() const {
    return p_e_oeminfo;
}

void
SgAsmDOSExtendedHeader::set_e_oeminfo(unsigned const& x) {
    this->p_e_oeminfo = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_res2() const {
    return p_e_res2;
}

void
SgAsmDOSExtendedHeader::set_e_res2(unsigned const& x) {
    this->p_e_res2 = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_res3() const {
    return p_e_res3;
}

void
SgAsmDOSExtendedHeader::set_e_res3(unsigned const& x) {
    this->p_e_res3 = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_res4() const {
    return p_e_res4;
}

void
SgAsmDOSExtendedHeader::set_e_res4(unsigned const& x) {
    this->p_e_res4 = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_res5() const {
    return p_e_res5;
}

void
SgAsmDOSExtendedHeader::set_e_res5(unsigned const& x) {
    this->p_e_res5 = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSExtendedHeader::get_e_res6() const {
    return p_e_res6;
}

void
SgAsmDOSExtendedHeader::set_e_res6(unsigned const& x) {
    this->p_e_res6 = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmDOSExtendedHeader::get_e_lfanew() const {
    return p_e_lfanew;
}

void
SgAsmDOSExtendedHeader::set_e_lfanew(Rose::BinaryAnalysis::Address const& x) {
    this->p_e_lfanew = x;
    set_isModified(true);
}

SgAsmDOSExtendedHeader::~SgAsmDOSExtendedHeader() {
    destructorHelper();
}

SgAsmDOSExtendedHeader::SgAsmDOSExtendedHeader()
    : p_e_res1(0)
    , p_e_oemid(0)
    , p_e_oeminfo(0)
    , p_e_res2(0)
    , p_e_res3(0)
    , p_e_res4(0)
    , p_e_res5(0)
    , p_e_res6(0)
    , p_e_lfanew(0) {}

void
SgAsmDOSExtendedHeader::initializeProperties() {
    p_e_res1 = 0;
    p_e_oemid = 0;
    p_e_oeminfo = 0;
    p_e_res2 = 0;
    p_e_res3 = 0;
    p_e_res4 = 0;
    p_e_res5 = 0;
    p_e_res6 = 0;
    p_e_lfanew = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
