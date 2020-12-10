#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Diagnostics.h"

using namespace Rose::Diagnostics;

SgAsmFloatValueExpression::SgAsmFloatValueExpression(double value, SgAsmType *type) {
    ASSERT_not_null(type);
    p_nativeValue = value;
    p_nativeValueIsValid = true;
    p_type = type;
    updateBitVector();
}

SgAsmFloatValueExpression::SgAsmFloatValueExpression(const Sawyer::Container::BitVector &bv, SgAsmType *type) {
    ASSERT_not_null(type);
    ASSERT_require(bv.size() == type->get_nBits());
    p_nativeValue = 0.0;
    p_nativeValueIsValid = false;
    p_bitVector = bv;
    updateNativeValue();
}

double
SgAsmFloatValueExpression::get_nativeValue() const {
    ASSERT_require2(p_nativeValueIsValid, "not implemented yet");
    return p_nativeValue;
}

void
SgAsmFloatValueExpression::set_nativeValue(double value) {
    p_nativeValue = value;
    p_nativeValueIsValid = true;
    updateBitVector();
}

void
SgAsmFloatValueExpression::updateBitVector() {
    // FIXME[Robb P. Matzke 2014-07-22]: not implemented yet.
    // Ultimately, this function should copy the native value from p_nativeValue into p_bitVector in the parent class,
    // performing whatever transformations are necessary to convert the native bits to the specified type.  We could borrow
    // code from HDF5 that does this, including the code that detects the native format.
    if (p_nativeValueIsValid) {
        static bool emitted = false;
        if (!emitted) {
            mlog[WARN] <<"SgAsmFloatValueExpression::updateBitVector is not implemented yet\n";
            emitted = true;
        }
    }
}

// Update the native value cache.  This is const because the cache is sometimes updated from methods like get_nativeValue().
void
SgAsmFloatValueExpression::updateNativeValue() const {
    if (!p_nativeValueIsValid) {
        static bool emitted = false;
        if (!emitted) {
            mlog[WARN] <<"SgAsmFloatValueExpression::updateNativeValue is not implemented yet\n";
            emitted = true;
        }
    }
}

#endif
