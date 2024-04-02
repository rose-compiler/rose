//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgJovialBitVal            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#define ROSE_SgJovialBitVal_IMPL
#include <sage3basic.h>

std::string const&
SgJovialBitVal::get_valueString() const {
    return p_valueString;
}

void
SgJovialBitVal::set_valueString(std::string const& x) {
    this->p_valueString = x;
    set_isModified(true);
}

SgJovialBitVal::~SgJovialBitVal() {
    destructorHelper();
}

SgJovialBitVal::SgJovialBitVal() {}

// The association between constructor arguments and their classes:
//    property=valueString      class=SgJovialBitVal
SgJovialBitVal::SgJovialBitVal(std::string const& valueString)
    : p_valueString(valueString) {}

void
SgJovialBitVal::initializeProperties() {
}

