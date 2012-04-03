// ROSETTA has no way to exclude certain IR node definitions based on the user's choice of languages.  Therefore, we need to
// provide (stub) definitions for those nodes' virtual methods in order to get things to compile when those nodes aren't
// actually being used anywhere.
#include "sage3basic.h"

uint64_t SgAsmIntegerValueExpression::virtual_address(SgNode*) { abort(); }
std::string SgAsmIntegerValueExpression::get_label(bool) const { abort(); }
void SgAsmIntegerValueExpression::set_significant_bits(size_t) { abort(); }
size_t SgAsmIntegerValueExpression::get_significant_bits() const { abort(); }
void SgAsmIntegerValueExpression::make_relative_to(SgNode*) { abort(); }
uint64_t SgAsmIntegerValueExpression::get_base_address() const { abort(); }
uint64_t SgAsmIntegerValueExpression::get_absolute_value(size_t) const { abort(); }
int64_t SgAsmIntegerValueExpression::get_absolute_signed_value() const { abort(); }
void SgAsmIntegerValueExpression::set_absolute_value(uint64_t) { abort(); }

uint8_t SgAsmByteValueExpression::get_value() const { abort(); }
uint16_t SgAsmWordValueExpression::get_value() const { abort(); }
uint32_t SgAsmDoubleWordValueExpression::get_value() const { abort(); }
uint64_t SgAsmQuadWordValueExpression::get_value() const { abort(); }

void SgAsmByteValueExpression::set_value(uint8_t) { abort(); }
void SgAsmWordValueExpression::set_value(uint16_t) { abort(); }
void SgAsmDoubleWordValueExpression::set_value(uint32_t) { abort(); }
void SgAsmQuadWordValueExpression::set_value(uint64_t) { abort(); }
    
SgAsmType *SgAsmIntegerValueExpression::get_type() { abort(); }
SgAsmType *SgAsmByteValueExpression::get_type() { abort(); }
SgAsmType *SgAsmWordValueExpression::get_type() { abort(); }
SgAsmType *SgAsmDoubleWordValueExpression::get_type() { abort(); }
SgAsmType *SgAsmQuadWordValueExpression::get_type() { abort(); }
