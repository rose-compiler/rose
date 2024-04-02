//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmStackMapFrame            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmStackMapFrame_IMPL
#include <SgAsmJvmStackMapFrame.h>

SgAsmJvmStackMapVerificationTypePtrList const&
SgAsmJvmStackMapFrame::get_stack() const {
    return p_stack;
}

SgAsmJvmStackMapVerificationTypePtrList&
SgAsmJvmStackMapFrame::get_stack() {
    return p_stack;
}

void
SgAsmJvmStackMapFrame::set_stack(SgAsmJvmStackMapVerificationTypePtrList const& x) {
    this->p_stack = x;
    set_isModified(true);
}

SgAsmJvmStackMapVerificationTypePtrList const&
SgAsmJvmStackMapFrame::get_locals() const {
    return p_locals;
}

SgAsmJvmStackMapVerificationTypePtrList&
SgAsmJvmStackMapFrame::get_locals() {
    return p_locals;
}

void
SgAsmJvmStackMapFrame::set_locals(SgAsmJvmStackMapVerificationTypePtrList const& x) {
    this->p_locals = x;
    set_isModified(true);
}

uint8_t const&
SgAsmJvmStackMapFrame::get_frame_type() const {
    return p_frame_type;
}

void
SgAsmJvmStackMapFrame::set_frame_type(uint8_t const& x) {
    this->p_frame_type = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmStackMapFrame::get_offset_delta() const {
    return p_offset_delta;
}

void
SgAsmJvmStackMapFrame::set_offset_delta(uint16_t const& x) {
    this->p_offset_delta = x;
    set_isModified(true);
}

SgAsmJvmStackMapFrame::~SgAsmJvmStackMapFrame() {
    destructorHelper();
}

SgAsmJvmStackMapFrame::SgAsmJvmStackMapFrame()
    : p_frame_type(0)
    , p_offset_delta(0) {}

void
SgAsmJvmStackMapFrame::initializeProperties() {
    p_frame_type = 0;
    p_offset_delta = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
