#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmFloatValueExpression.h>

#include <Rose/Diagnostics.h>

#include <SgAsmType.h>

using namespace Rose::Diagnostics;

SgAsmFloatValueExpression::SgAsmFloatValueExpression(double value, SgAsmType *type) {
    initializeProperties();

    ASSERT_not_null(type);
    set_nativeValue(value);
    p_nativeValueIsValid = true;
    set_type(type);
    updateBitVector();
}

SgAsmFloatValueExpression::SgAsmFloatValueExpression(const Sawyer::Container::BitVector &bv, SgAsmType *type) {
    initializeProperties();

    ASSERT_always_not_null(type);
    ASSERT_require(bv.size() == type->get_nBits());
    set_nativeValue(0.0);
    p_nativeValueIsValid = false;
    set_bitVector(bv);
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
