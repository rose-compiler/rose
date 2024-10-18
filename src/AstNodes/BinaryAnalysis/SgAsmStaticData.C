#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

const SgUnsignedCharList&
SgAsmStaticData::get_raw_bytes() const {
    return get_rawBytes();
}

void
SgAsmStaticData::set_raw_bytes(const SgUnsignedCharList &x) {
    set_rawBytes(x);
}

size_t
SgAsmStaticData::get_size() const {
    return get_rawBytes().size();
}

#endif
