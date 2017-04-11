// Originally based on https://outreach.scidac.gov/svn/rose/trunk@891
// src/midend/programTransformation/constantFolding/constantFolding.C

#include "rosetollvm/ConstantFolder.h"
#include <string>

// These macros create a map of types and values from source code (S) to
// host (H) and to destination (D).
//
// For example:
//   - The source code might contain an SgEnumVal, which the folder can
//     detect using CF_ENUM_SVAL(is) or CF_ENUM_SVAL(V_).
//   - The folder must perform all computations on the host platform as
//     opposed to the target platform.  The set of functions for
//     performing such computations on an SgEnumVal have names with the
//     suffix "_integer".  Thus, if a particular function has the
//     prefix "calculate", its name is CF_ENUM_HFUNC(calculate).
//   - During the computation, the folder stores the value on the host
//     in an int, which is CF_ENUM_HTYPE.
//   - Utlimately, the folder can store the resulting folded value in an
//     SgIntVal, which is CF_ENUM_DVAL, using SageBuilder::buildIntVal,
//     which is CF_ENUM_BDVAL.
//   - As another example, if the source code specifies a cast from some
//     other type to an SgEnumType, the folder can detect the SgEnumType
//     using CF_ENUM_STYPE(is) or CF_ENUM_STYPE(V_).  During the cast,
//     the folder stores the value in a CF_ENUM_HTYPE, and it stores the
//     result in a CF_ENUM_DVAL using CF_ENUM_BDVAL, as before.

#define CF_BOOL_STYPE(P) P##SgTypeBool
#define CF_BOOL_SVAL(P)  P##SgBoolValExp
#define CF_BOOL_HFUNC(P) P##_integer
#define CF_BOOL_HTYPE    bool
#define CF_BOOL_DVAL     SgBoolValExp
#define CF_BOOL_DBVAL    SageBuilder::buildBoolValExp

#define CF_CHAR_STYPE(P) P##SgTypeChar
#define CF_CHAR_SVAL(P)  P##SgCharVal
#define CF_CHAR_HFUNC(P) P##_integer
#define CF_CHAR_HTYPE    char
#define CF_CHAR_DVAL     SgCharVal
#define CF_CHAR_DBVAL    SageBuilder::buildCharVal

// FIXME: The host and destination storage must not lose precision
// relative to the same computation on the target platform, but they can
// be greater according to C99.
#define CF_DOUBLE_STYPE(P) P##SgTypeDouble
#define CF_DOUBLE_SVAL(P)  P##SgDoubleVal
#define CF_DOUBLE_HFUNC(P) P##_float
#define CF_DOUBLE_HTYPE    double
#define CF_DOUBLE_DVAL     SgDoubleVal
#define CF_DOUBLE_DBVAL    SageBuilder::buildDoubleVal

#define CF_ENUM_STYPE(P) P##SgEnumType
#define CF_ENUM_SVAL(P)  P##SgEnumVal
#define CF_ENUM_HFUNC(P) P##_integer
#define CF_ENUM_HTYPE    int
#define CF_ENUM_DVAL     SgIntVal
#define CF_ENUM_DBVAL    SageBuilder::buildIntVal

// FIXME: The host and destination storage must not lose precision
// relative to the same computation on the target platform, but they can
// be greater according to C99.
#define CF_FLOAT_STYPE(P) P##SgTypeFloat
#define CF_FLOAT_SVAL(P)  P##SgFloatVal
#define CF_FLOAT_HFUNC(P) P##_float
#define CF_FLOAT_HTYPE    float
#define CF_FLOAT_DVAL     SgFloatVal
#define CF_FLOAT_DBVAL    SageBuilder::buildFloatVal

#define CF_INT_STYPE(P) P##SgTypeInt
#define CF_INT_SVAL(P)  P##SgIntVal
#define CF_INT_HFUNC(P) P##_integer
#define CF_INT_HTYPE    int
#define CF_INT_DVAL     SgIntVal
#define CF_INT_DBVAL    SageBuilder::buildIntVal

// FIXME: The host and destination storage must not lose precision
// relative to the same computation on the target platform, but they can
// be greater according to C99.
#define CF_LONG_DOUBLE_STYPE(P) P##SgTypeLongDouble
#define CF_LONG_DOUBLE_SVAL(P)  P##SgLongDoubleVal
#define CF_LONG_DOUBLE_HFUNC(P) P##_float
#define CF_LONG_DOUBLE_HTYPE    long double
#define CF_LONG_DOUBLE_DVAL     SgLongDoubleVal
#define CF_LONG_DOUBLE_DBVAL    SageBuilder::buildLongDoubleVal

#define CF_LONG_INT_STYPE(P) P##SgTypeLong
#define CF_LONG_INT_SVAL(P)  P##SgLongIntVal
#define CF_LONG_INT_HFUNC(P) P##_integer
#define CF_LONG_INT_HTYPE    long int
#define CF_LONG_INT_DVAL     SgLongIntVal
#define CF_LONG_INT_DBVAL    SageBuilder::buildLongIntVal

#define CF_LONG_LONG_INT_STYPE(P) P##SgTypeLongLong
#define CF_LONG_LONG_INT_SVAL(P)  P##SgLongLongIntVal
#define CF_LONG_LONG_INT_HFUNC(P) P##_integer
#define CF_LONG_LONG_INT_HTYPE    long long int
#define CF_LONG_LONG_INT_DVAL     SgLongLongIntVal
#define CF_LONG_LONG_INT_DBVAL    SageBuilder::buildLongLongIntVal

#define CF_SHORT_STYPE(P) P##SgTypeShort
#define CF_SHORT_SVAL(P)  P##SgShortVal
#define CF_SHORT_HFUNC(P) P##_integer
#define CF_SHORT_HTYPE    short
#define CF_SHORT_DVAL     SgShortVal
#define CF_SHORT_DBVAL    SageBuilder::buildShortVal

#define CF_STRING_STYPE(P) P##SgTypeString
#define CF_STRING_SVAL(P)  P##SgStringVal
#define CF_STRING_HFUNC(P) P##_string
#define CF_STRING_HTYPE    std::string
#define CF_STRING_DVAL     SgStringVal
#define CF_STRING_DBVAL    SageBuilder::buildStringVal

#define CF_UNSIGNED_CHAR_STYPE(P) P##SgTypeUnsignedChar
#define CF_UNSIGNED_CHAR_SVAL(P)  P##SgUnsignedCharVal
#define CF_UNSIGNED_CHAR_HFUNC(P) P##_integer
#define CF_UNSIGNED_CHAR_HTYPE    unsigned char
#define CF_UNSIGNED_CHAR_DVAL     SgUnsignedCharVal
#define CF_UNSIGNED_CHAR_DBVAL    SageBuilder::buildUnsignedCharVal

#define CF_UNSIGNED_INT_STYPE(P) P##SgTypeUnsignedInt
#define CF_UNSIGNED_INT_SVAL(P)  P##SgUnsignedIntVal
#define CF_UNSIGNED_INT_HFUNC(P) P##_integer
#define CF_UNSIGNED_INT_HTYPE    unsigned int
#define CF_UNSIGNED_INT_DVAL     SgUnsignedIntVal
#define CF_UNSIGNED_INT_DBVAL    SageBuilder::buildUnsignedIntVal

#define CF_UNSIGNED_LONG_STYPE(P) P##SgTypeUnsignedLong
#define CF_UNSIGNED_LONG_SVAL(P)  P##SgUnsignedLongVal
#define CF_UNSIGNED_LONG_HFUNC(P) P##_integer
#define CF_UNSIGNED_LONG_HTYPE    unsigned long
#define CF_UNSIGNED_LONG_DVAL     SgUnsignedLongVal
#define CF_UNSIGNED_LONG_DBVAL    SageBuilder::buildUnsignedLongVal

#define CF_UNSIGNED_LONG_LONG_STYPE(P)  P##SgTypeUnsignedLongLong
#define CF_UNSIGNED_LONG_LONG_SVAL(P)   P##SgUnsignedLongLongIntVal
#define CF_UNSIGNED_LONG_LONG_HFUNC(P)  P##_integer
#define CF_UNSIGNED_LONG_LONG_HTYPE     unsigned long long int
#define CF_UNSIGNED_LONG_LONG_DVAL      SgUnsignedLongLongIntVal
#define CF_UNSIGNED_LONG_LONG_DBVAL \
  SageBuilder::buildUnsignedLongLongIntVal

#define CF_UNSIGNED_SHORT_STYPE(P) P##SgTypeUnsignedShort
#define CF_UNSIGNED_SHORT_SVAL(P)  P##SgUnsignedShortVal
#define CF_UNSIGNED_SHORT_HFUNC(P) P##_integer
#define CF_UNSIGNED_SHORT_HTYPE    unsigned short
#define CF_UNSIGNED_SHORT_DVAL     SgUnsignedShortVal
#define CF_UNSIGNED_SHORT_DBVAL    SageBuilder::buildUnsignedShortVal

ConstantFolder::SynthesizedAttribute &
ConstantFolder::SynthesizedAttribute::operator=(
  SynthesizedAttribute const &other
) {
  if (this != &other) {
    clear();
    expr = other.expr;
    refCount = other.refCount;
    if (refCount) { ++*refCount; }
    castType = other.castType;
    byteOffset = other.byteOffset;
    omitParentAddressOfOp = other.omitParentAddressOfOp;
    attributes = other.attributes;
  }
  return *this;
}

void
ConstantFolder::SynthesizedAttribute::setNewExpression(
  SgExpression *e
) {
  ROSE2LLVM_ASSERT(attributes);
  setOldExpression(e);
  refCount = new unsigned long;
  *refCount = 1;
}

SgExpression *
ConstantFolder::SynthesizedAttribute::completeExpression() const
{
  ROSE2LLVM_ASSERT(attributes);
  if (!expr) return NULL;
  SgExpression *result;
  {
    SgTreeCopy copier;
    result = (SgExpression*)expr->copy(copier);
  }
  if (byteOffset) {
    result = SageBuilder::buildCastExp(
      result,
      SageBuilder::buildPointerType(SageBuilder::buildCharType())
    );
    result = SageBuilder::buildAddOp(
      result,
      SageBuilder::buildLongLongIntVal(byteOffset)
    );
  }
  SgType *result_type = castType ? castType : expr->get_type();
  // This condition is a little optimization that is not necessary for
  // correctness and that doesn't always work because ROSE sometimes
  // constructs multiple type nodes for the same type.  Dan Quinlan is
  // not sure whether that should be considered a bug.
  if (result_type != result->get_type()) {
    result = SageBuilder::buildCastExp(result, result_type);
  }
  return result;
}

void
ConstantFolder::SynthesizedAttribute::clear()
{
  if (refCount) {
    if (1 < *refCount) {
      --*refCount;
    }
    else {
      ROSE2LLVM_ASSERT(*refCount == 1);
      SageInterface::deepDelete(expr);
      delete refCount;
    }
    refCount = NULL;
  }
  expr = NULL;
  castType = NULL;
  byteOffset = 0;
  omitParentAddressOfOp = false;
}

// Calculate the result of a unary operation on a constant.
template<typename T>
static T
calculate_integer(SgUnaryOp const *op, T operand)
{
  T result;
  switch (op->variantT()) {
    case V_SgBitComplementOp: result = ~operand; break;
    case V_SgMinusOp:         result = -operand; break;
    case V_SgNotOp:           result = !operand; break;
    default: {
      std::cerr << "ConstantFolder: " << op->class_name()
                << " not implemented for integer" << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}

// Calculate the result of a unary operation on a constant.
template<typename T>
static T
calculate_float(SgUnaryOp const *op, T operand)
{
  T result;
  switch (op->variantT()) {
    case V_SgMinusOp:         result = -operand; break;
    case V_SgNotOp:           result = !operand; break;
    default: {
      std::cerr << "ConstantFolder: " << op->class_name()
                << " not implemented for float" << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldArithmeticUnaryOp(
  SgUnaryOp const *op, SynthesizedAttribute operand_attr
) {
  SynthesizedAttribute result(attributes);
  SgExpression const *operand_expr = operand_attr.getExpression();
  if (
    isSgValueExp(operand_expr) && !isSgStringVal(operand_expr)
    && !operand_attr.getCastType()
  ) {
    switch (operand_expr->variantT()) {
      #define CF_CASE(T) \
      case CF_##T##_SVAL(V_): { \
        result.setNewExpression(CF_##T##_DBVAL( \
          CF_##T##_HFUNC(calculate)( \
            op, CF_##T##_SVAL(is)(operand_expr)->get_value() \
          ) \
        )); \
        break; \
      }
      CF_CASE(BOOL)
      CF_CASE(CHAR)
      CF_CASE(DOUBLE)
      CF_CASE(ENUM)
      CF_CASE(FLOAT)
      CF_CASE(INT)
      CF_CASE(LONG_DOUBLE)
      CF_CASE(LONG_INT)
      CF_CASE(LONG_LONG_INT)
      CF_CASE(SHORT)
      CF_CASE(UNSIGNED_CHAR)
      CF_CASE(UNSIGNED_INT)
      CF_CASE(UNSIGNED_LONG)
      CF_CASE(UNSIGNED_LONG_LONG)
      CF_CASE(UNSIGNED_SHORT)
      #undef CF_CASE
      default: {
        std::cerr << "ConstantFolder: " << op->class_name()
                  << " not implemented for "
                  << operand_expr->class_name() << std::endl;
        ROSE2LLVM_ASSERT(false);
        break;
      }
    }
  }
  else {
    SgShallowCopy copier;
    SgUnaryOp *op_new = (SgUnaryOp*)op->copy(copier);
    op_new->set_operand(operand_attr.completeExpression());
    result.setNewExpression(op_new);
  }
  return result;
}

template <typename ResultType>
static ResultType
cast(SgValueExp const *value)
{
  ResultType result;
  switch (value->variantT()) {
    #define CF_CASE(T) \
    case CF_##T##_SVAL(V_): { \
      result = (ResultType) CF_##T##_SVAL(is)(value)->get_value();	\
      break; \
    }
    CF_CASE(BOOL)
    CF_CASE(CHAR)
    CF_CASE(DOUBLE)
    CF_CASE(ENUM)
    CF_CASE(FLOAT)
    CF_CASE(INT)
    CF_CASE(LONG_DOUBLE)
    CF_CASE(LONG_INT)
    CF_CASE(LONG_LONG_INT)
    CF_CASE(SHORT)
    CF_CASE(UNSIGNED_CHAR)
    CF_CASE(UNSIGNED_INT)
    CF_CASE(UNSIGNED_LONG)
    CF_CASE(UNSIGNED_LONG_LONG)
    CF_CASE(UNSIGNED_SHORT)
    #undef CF_CASE
    default: {
      std::cerr << "ConstantFolder: cast not implemented for "
                << value->class_name() << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldCast(
  SgType *result_type, SynthesizedAttribute operand_attr,
  bool scale_for_pointer
) {
  SynthesizedAttribute result(attributes);
  SgType *result_type_stripped =
    result_type->stripTypedefsAndModifiers();
  if (
    operand_attr.hasIntegerBase()
    && isSgPointerType(result_type_stripped)
  ) {
    // Scale the integer by the pointer base type only if requested and
    // only if that integer wasn't previously cast to a pointer (because
    // that would mean this is the pointer operand of an addition).
    size_t base_size = 1;
    if (scale_for_pointer && !operand_attr.getCastType()) {
      // We need the LLVM_SIZE for cases like pointer + integer.
      // FIXME: setLLVMTypeName assumes that the host platform is the
      // target platform.
      SgType *base_type =
        isSgPointerType(result_type_stripped)->get_base_type();
      attributes->setLLVMTypeName(base_type);
      base_size =
        ((IntAstAttribute*)base_type->getAttribute(Control::LLVM_SIZE))
          ->getValue();
      if (!base_size) base_size = 1;
    }
    attributes->setLLVMTypeName(result_type);
    size_t ptr_size =
      ((IntAstAttribute*)result_type->getAttribute(Control::LLVM_SIZE))
        ->getValue();
    if (false) ;
    // FIXME: This isn't right for cross-compilation.  For example,
    // unsigned long long might not have the same size on the host and
    // target platform.  That is, HTYPE and DBVAL might have to be
    // different types.  Of course, what do we do if DBVAL isn't
    // representable on the host?
    #define CF_CASE(T) \
    else if (ptr_size == sizeof(CF_##T##_HTYPE)) { \
      result.setNewExpression( \
        CF_##T##_DBVAL( \
          base_size * cast<CF_##T##_HTYPE>( \
            isSgValueExp(operand_attr.getExpression()) \
          ) \
        ) \
      ); \
    }
    // We can't use a switch because sizeof is not guaranteed to
    // give different sizes in all these cases and a switch cannot
    // have two cases with the same value.
    CF_CASE(UNSIGNED_INT)
    CF_CASE(UNSIGNED_LONG)
    CF_CASE(UNSIGNED_LONG_LONG)
    #undef CF_CASE
    else {
      std::cerr << "ConstantFolder: void* has an unexpected size"
                << std::endl;
      ROSE2LLVM_ASSERT(false);
    }
    result.setCastType(result_type);
  }
  else if (
    operand_attr.hasNonZeroAddressBase()
    && isSgTypeBool(result_type_stripped)
  ) {
    result.setNewExpression(SageBuilder::buildBoolValExp(1));
  }
  else if (
    operand_attr.hasAddressBase()
    && result.canTypeStorePointer(result_type)
  ) {
    result = operand_attr;
    // It's possible that the type of the stored expression will be the
    // same as the cast type, but that shouldn't hurt anything.
    result.setCastType(result_type);
  }
  else if (
    isSgValueExp(operand_attr.getExpression())
    && !isSgStringVal(operand_attr.getExpression())
  ) {
    switch (result_type_stripped->variantT()) {
      #define CF_CASE(T) \
      case CF_##T##_STYPE(V_): { \
        result.setNewExpression(CF_##T##_DBVAL( \
          cast<CF_##T##_HTYPE>( \
            isSgValueExp(operand_attr.getExpression()) \
          ) \
        )); \
        break; \
      }
      CF_CASE(BOOL)
      CF_CASE(CHAR)
      CF_CASE(DOUBLE)
      CF_CASE(ENUM)
      CF_CASE(FLOAT)
      CF_CASE(INT)
      CF_CASE(LONG_DOUBLE)
      CF_CASE(LONG_INT)
      CF_CASE(LONG_LONG_INT)
      CF_CASE(SHORT)
      CF_CASE(UNSIGNED_CHAR)
      CF_CASE(UNSIGNED_INT)
      CF_CASE(UNSIGNED_LONG)
      CF_CASE(UNSIGNED_LONG_LONG)
      CF_CASE(UNSIGNED_SHORT)
      #undef CF_CASE
      default: {
        std::cerr << "ConstantFolder: cast to "
                  << result_type->class_name() << " not implemented"
                  << std::endl;
        ROSE2LLVM_ASSERT(false);
        break;
      }
    }
  }
  else {
    result.setNewExpression(SageBuilder::buildCastExp(
      operand_attr.completeExpression(), result_type
    ));
  }
  return result;
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldPointerDerefExp(
  SynthesizedAttribute operand_attr, SgNode const *parent
) {
  SynthesizedAttribute result(attributes);
  SgType *type =
    operand_attr.getCompleteType()->stripTypedefsAndModifiers();
  SgArrayType const *array_type = isSgArrayType(type);
  SgPointerType const *ptr_type = isSgPointerType(type);
  SgType *base_type_orig = NULL;
  if (array_type) base_type_orig = array_type->get_base_type();
  else if (ptr_type) base_type_orig = ptr_type->get_base_type();
  SgType *base_type = NULL;
  if (base_type_orig) {
    base_type = base_type_orig->stripTypedefsAndModifiers();
  }

  bool folded = false;
  // If the parent is an SgAddressOfOp, then fold by simply removing
  // both the parent operation and this operation.  The ability to
  // do that could be lost if we attempted any of the following ways
  // of folding.
  if (isSgAddressOfOp(parent) || isSgDotExp(parent)) {
    result = operand_attr;
    result.setOmitParentAddressOfOp();
    folded = true;
  }
  // Dereferencing a function type has no effect.
  else if (isSgFunctionType(type)) {
    result = operand_attr;
    folded = true;
  }
  // Though C99 doesn't seem to require that dereferencing of
  // literal strings be permitted in constant expressions, ROSE
  // permits it, so fold it by returning a char.  However, if
  // there's a cast, attempt this way of folding only if it's a
  // cast to a pointer to some kind of char.  (Otherwise, the next
  // kind of folding is for an array base type, and it might still
  // be possible.)
  else if (
    isSgStringVal(operand_attr.getExpression())
    && (
      isSgTypeString(type)
      || (
        ptr_type
        && (
          isSgTypeChar(base_type) || isSgTypeSignedChar(base_type)
          || isSgTypeUnsignedChar(base_type)
        )
      )
    )
  ) {
    // Fold only if any offset is within the string's bounds.
    std::string value =
      isSgStringVal(operand_attr.getExpression())->get_value();
    if (operand_attr.getByteOffset() <= value.size()) {
      result.setNewExpression(SageBuilder::buildCharVal(
        value[operand_attr.getByteOffset()]
      ));
      folded = true;
    }
  }
  // When the operand is an array of arrays or a pointer to an
  // array, a dereference can be rewritten as a cast.  However,
  // don't bother to rewrite as a cast unless the operand is
  // actually an address constant, for which we're allowed to call
  // setCastType.
  else if (
    (array_type || ptr_type) && isSgArrayType(base_type)
    && operand_attr.hasConstantBase()
  ) {
    result = operand_attr;
    result.setCastType(base_type_orig);
    folded = true;
  }
  if (!folded) {
    result.setNewExpression(SageBuilder::buildPointerDerefExp(
      operand_attr.completeExpression()
    ));
  }
  return result;
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldUnaryOp(
  SgUnaryOp const *op, SynthesizedAttribute operand_attr
) {
  SynthesizedAttribute result(attributes);
  switch (op->variantT()) {
    case V_SgAddressOfOp: {
      if (operand_attr.popOmitParentAddressOfOp()) {
        result = operand_attr;
      }
      else {
        result.setNewExpression(SageBuilder::buildAddressOfOp(
          operand_attr.completeExpression()
        ));
      }
      break;
    }
    case V_SgBitComplementOp: {
      result = foldArithmeticUnaryOp(op, operand_attr);
      break;
    }
    case V_SgCastExp: {
      result = foldCast(op->get_type(), operand_attr);
      break;
    }
    case V_SgMinusOp: {
      result = foldArithmeticUnaryOp(op, operand_attr);
      break;
    }
    case V_SgNotOp: {
      if (operand_attr.hasNonZeroAddressBase()) {
        result.setNewExpression(SageBuilder::buildBoolValExp(0));
      }
      else {
        result = foldArithmeticUnaryOp(op, operand_attr);
      }
      break;
    }
    case V_SgPointerDerefExp: {
      result = foldPointerDerefExp(operand_attr, op->get_parent());
      break;
    }
    case V_SgUnaryAddOp: {
      // FIXME: A lot of our folding for arithmetic expressions, such as
      // those involving unary plus, is wrong because we haven't
      // implemented what C99 calls "integer promotions'.  For example,
      // sizeof(+(char)'a') should be sizeof(int) not sizeof(char).
      result = operand_attr;
      break;
    }
    // C99 explicitly disallows increment and decrement operators to be
    // evaluated in constant expressions, so don't try to fold them.
    case V_SgMinusMinusOp:
    case V_SgPlusPlusOp: {
      SgShallowCopy copier;
      SgUnaryOp *op_new = (SgUnaryOp*)op->copy(copier);
      op_new->set_operand(operand_attr.completeExpression());
      result.setNewExpression(op_new);
      break;
    }
    // Complex numbers are not yet implemented.
    case V_SgConjugateOp:
    case V_SgImagPartOp:
    case V_SgRealPartOp:
    // C++ is not implemented.
    case V_SgThrowOp:
    case V_SgUserDefinedUnaryOp:
    // Unused according to comments in ROSE.
    case V_SgExpressionRoot:
    default: {
      std::cerr << "ConstantFolder: unimplemented unary operator: "
                << op->class_name() << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}

// Calculate the result of a binary operation on two constant
// integer-type values.  FIXME: What if T is a different size on the
// target platform?
template<typename T>
static T
calculate_integer(SgBinaryOp const *op, T lhs, T rhs)
{
  T result;
  switch (op->variantT()) {
    // integer-exclusive oprations
    case V_SgModOp:            result = lhs %  rhs; break;
    case V_SgLshiftOp:         result = lhs << rhs; break;
    case V_SgRshiftOp:         result = lhs >> rhs; break;
    // bitwise operations
    case V_SgBitAndOp:         result = lhs &  rhs; break;
    case V_SgBitOrOp:          result = lhs |  rhs; break;
    case V_SgBitXorOp:         result = lhs ^  rhs; break;
    // non-integer-exclusive operations
    case V_SgAddOp:            result = lhs +  rhs; break;
    case V_SgSubtractOp:       result = lhs -  rhs; break;
    case V_SgMultiplyOp:       result = lhs *  rhs; break;
    case V_SgDivideOp:         result = lhs /  rhs; break;
    //TODO what is this ??
    case V_SgIntegerDivideOp:  result = lhs /  rhs; break;
    // logic operations
    case V_SgAndOp:            result = lhs && rhs; break;
    case V_SgOrOp:             result = lhs || rhs; break;
    // relational operations
    case V_SgEqualityOp:       result = lhs == rhs; break;
    case V_SgNotEqualOp:       result = lhs != rhs; break;
    case V_SgGreaterOrEqualOp: result = lhs >= rhs; break;
    case V_SgGreaterThanOp:    result = lhs >  rhs; break;
    case V_SgLessOrEqualOp:    result = lhs <= rhs; break;
    case V_SgLessThanOp:       result = lhs <  rhs; break;
    default: {
      std::cerr << "ConstantFolder: " << op->class_name()
                << " not implemented for integer" << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}

// Calculate the result of a binary operation on two constant
// float-kind values.  FIXME: What if T is a different size on the
// target platform?
template<typename T>
static T
calculate_float(SgBinaryOp const *op, T lhs, T rhs)
{
  T result;
  switch (op->variantT()) {
    // Basic arithmetic
    case V_SgAddOp:            result = lhs +  rhs; break;
    case V_SgSubtractOp:       result = lhs -  rhs; break;
    case V_SgMultiplyOp:       result = lhs *  rhs; break;
    case V_SgDivideOp:         result = lhs /  rhs; break;
    //TODO what is this ??
    case V_SgIntegerDivideOp:  result = lhs /  rhs; break;
    // logic operations
    case V_SgAndOp:            result = lhs && rhs; break;
    case V_SgOrOp:             result = lhs || rhs; break;
    // relational operations
    case V_SgEqualityOp:       result = lhs == rhs; break;
    case V_SgNotEqualOp:       result = lhs != rhs; break;
    case V_SgGreaterOrEqualOp: result = lhs >= rhs; break;
    case V_SgGreaterThanOp:    result = lhs >  rhs; break;
    case V_SgLessOrEqualOp:    result = lhs <= rhs; break;
    case V_SgLessThanOp:       result = lhs <  rhs; break;
    default: {
      std::cerr << "ConstantFolder: " << op->class_name()
                << " not implemented for floating point" << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}


ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldArithmeticBinaryOp(
  SgBinaryOp const *op, SynthesizedAttribute lhs_attr,
  SynthesizedAttribute rhs_attr, bool scale_for_pointer
) {
  // op and its children might be freed by the caller immediately upon
  // return.  Its only use here is for its node type and its expression
  // type.
  SynthesizedAttribute result(attributes);
  if (
    isSgValueExp(lhs_attr.getExpression())
    && !isSgStringVal(lhs_attr.getExpression())
    && isSgValueExp(rhs_attr.getExpression())
    && !isSgStringVal(rhs_attr.getExpression())
  ) {
    // If the operands are not of the same type as the operator, just
    // cast them to that type.  For pointer arithmetic, we also scale
    // the integer operand.  FIXME: Are there any other cases that need
    // special handling?  What does C99 say about all this?  FIXME: ROSE
    // does not always guarantee that identical types will have
    // identical addresses (and Dan Quinlan is not sure if that's a
    // bug), but an extra cast won't hurt in that case.
    if (lhs_attr.getCompleteType() != op->get_type()) {
      lhs_attr = foldCast(op->get_type(), lhs_attr, scale_for_pointer);
      ROSE2LLVM_ASSERT(isSgValueExp(lhs_attr.getExpression()));
    }
    if (rhs_attr.getCompleteType() != op->get_type()) {
      rhs_attr = foldCast(op->get_type(), rhs_attr, scale_for_pointer);
      ROSE2LLVM_ASSERT(isSgValueExp(rhs_attr.getExpression()));
    }
    // If either lhs_attr or rhs_attr now has a cast type, it must be a
    // pointer type, and so both must have that cast type, and each of
    // their stored expressions must be an integer SgValueExp of the
    // same size as a pointer.  If, instead, they don't have cast types,
    // then the kind of SgValueExp still must be the same.
    ROSE2LLVM_ASSERT(
      lhs_attr.getExpression()->variantT()
      == rhs_attr.getExpression()->variantT()
    );
    switch (
      lhs_attr.getExpression()->variantT()
    ) {
      #define CF_CASE(T) \
      case CF_##T##_SVAL(V_): { \
        CF_##T##_HTYPE lhs = \
          CF_##T##_SVAL(is)(lhs_attr.getExpression())->get_value(); \
        CF_##T##_HTYPE rhs = \
          CF_##T##_SVAL(is)(rhs_attr.getExpression())->get_value(); \
        result.setNewExpression( \
          CF_##T##_DBVAL(CF_##T##_HFUNC(calculate)(op, lhs, rhs)) \
        ); \
        break; \
      }
      CF_CASE(BOOL)
      CF_CASE(CHAR)
      CF_CASE(DOUBLE)
      CF_CASE(ENUM)
      CF_CASE(FLOAT)
      CF_CASE(INT)
      CF_CASE(LONG_DOUBLE)
      CF_CASE(LONG_INT)
      CF_CASE(LONG_LONG_INT)
      CF_CASE(SHORT)
      CF_CASE(UNSIGNED_CHAR)
      CF_CASE(UNSIGNED_INT)
      CF_CASE(UNSIGNED_LONG)
      CF_CASE(UNSIGNED_LONG_LONG)
      CF_CASE(UNSIGNED_SHORT)
      #undef CF_CASE
      default: {
        std::cerr << "ConstantFolder: binary operations not implemented"
                  << " for " << lhs_attr.getExpression()->class_name()
                  << std::endl;
        ROSE2LLVM_ASSERT(false);
        break;
      }
    }
    if (lhs_attr.getCastType()) {
      result.setCastType(lhs_attr.getCastType());
    }
  }
  else if (
    (
      lhs_attr.hasAddressBase()
      && (isSgAddOp(op) || isSgSubtractOp(op))
      && rhs_attr.hasIntegerBase() && !rhs_attr.getCastType()
    )
    || (
      lhs_attr.hasIntegerBase() && !lhs_attr.getCastType()
      && isSgAddOp(op) && rhs_attr.hasAddressBase()
    )
  ) {
    // FIXME: Clearly the integer operand might need to be promoted to
    // the size of a pointer.  However, if the address operand has
    // already been cast to an integer type that's smaller than the
    // integer operand's type, the address might need to be promoted
    // too.  Unfortunately, I'm not so sure how that reduces to C99's
    // required object address plus integer expression.  That is, this
    // performs arithmetic in a larger space than pointer arithmetic
    // would be performed, so it is less apt to overflow.  Does that
    // matter?
    if (lhs_attr.getCompleteType() != op->get_type()) {
      lhs_attr = foldCast(op->get_type(), lhs_attr, scale_for_pointer);
    }
    if (rhs_attr.getCompleteType() != op->get_type()) {
      rhs_attr = foldCast(op->get_type(), rhs_attr, scale_for_pointer);
    }
    SynthesizedAttribute &ptr_attr =
      lhs_attr.hasAddressBase() ? lhs_attr : rhs_attr;
    SynthesizedAttribute &integer_attr =
      lhs_attr.hasAddressBase() ? rhs_attr : lhs_attr;
    ROSE2LLVM_ASSERT(isSgValueExp(integer_attr.getExpression()));
    // FIXME: Storing the offset always as a long long seems wrong.  It
    // seems like it ought to depend on the source type.
    long long offset;
    switch (
      integer_attr.getExpression()->variantT()
    ) {
      #define CF_CASE(T) \
      case CF_##T##_SVAL(V_): { \
        offset = \
          CF_##T##_SVAL(is)(integer_attr.getExpression()) \
            ->get_value(); \
        break; \
      }
      CF_CASE(UNSIGNED_INT)
      CF_CASE(UNSIGNED_LONG)
      CF_CASE(UNSIGNED_LONG_LONG)
      #undef CF_CASE
      default: {
        std::cerr << "ConstantFolder: unexpected integer type in"
                  << " pointer arithmetic" << std::endl;
        ROSE2LLVM_ASSERT(false);
        break;
      }
    }
    if (isSgSubtractOp(op)) {
      offset = -offset;
    }
    result = ptr_attr;
    result.setByteOffset(result.getByteOffset() + offset);
  }
  else {
    SgShallowCopy copier;
    SgBinaryOp *op_new = (SgBinaryOp*)op->copy(copier);
    op_new->set_lhs_operand(lhs_attr.completeExpression());
    op_new->set_rhs_operand(rhs_attr.completeExpression());
    result.setNewExpression(op_new);
  }
  return result;
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldArrowExp(
  SgBinaryOp const *op, SynthesizedAttribute lhs_attr,
  SynthesizedAttribute rhs_attr
) {
  SgExpression *address_expr = lhs_attr.completeExpression();
  SgValueExp *offset_expr;
  {
    attributes->setLLVMTypeName(address_expr->get_type());
    IntAstAttribute *offset_ast_attr =
      (IntAstAttribute*)
      isSgVarRefExp(op->get_rhs_operand())->get_symbol()
        ->get_declaration()->getAttribute(Control::LLVM_FIELD_OFFSET);
    ROSE2LLVM_ASSERT(offset_ast_attr);
    offset_expr =
      SageBuilder::buildLongLongIntVal(offset_ast_attr->getValue());
  }
  SynthesizedAttribute offset_attr(attributes);
  offset_attr.setNewExpression(offset_expr);
  // This needs non-NULL operands so add_op->get_type() will compute
  // a type.
  SgAddOp *add_op = SageBuilder::buildAddOp(address_expr, offset_expr);
  SynthesizedAttribute result =
    foldArithmeticBinaryOp(add_op, lhs_attr, offset_attr, false);
  delete add_op;
  delete address_expr;
  result = foldCast(
    SageBuilder::buildPointerType(
      isSgVarRefExp(op->get_rhs_operand())->get_type()
    ),
    result
  );
  return foldPointerDerefExp(result, op->get_parent());
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldBinaryOp(
  SgBinaryOp const *op, SynthesizedAttribute lhs_attr,
  SynthesizedAttribute rhs_attr
) {
  SynthesizedAttribute result(attributes);
  switch (op->variantT()) {
    case V_SgAddOp:
    case V_SgAndOp:
    case V_SgBitAndOp:
    case V_SgBitOrOp:
    case V_SgBitXorOp:
    case V_SgDivideOp:
    case V_SgEqualityOp:
    case V_SgGreaterOrEqualOp:
    case V_SgGreaterThanOp:
    case V_SgIntegerDivideOp:
    case V_SgLessOrEqualOp:
    case V_SgLessThanOp:
    case V_SgLshiftOp:
    case V_SgModOp:
    case V_SgMultiplyOp:
    case V_SgNotEqualOp:
    case V_SgOrOp:
    case V_SgRshiftOp:
    case V_SgSubtractOp: {
      result = foldArithmeticBinaryOp(op, lhs_attr, rhs_attr);
      break;
    }
    // C99 explicitly disallows assignment and comma operators to
    // evaluated in constant expressions, so don't try to fold them.
    case V_SgAndAssignOp:
    case V_SgAssignOp:
    case V_SgCommaOpExp:
    case V_SgDivAssignOp:
    case V_SgIorAssignOp:
    case V_SgLshiftAssignOp:
    case V_SgMinusAssignOp:
    case V_SgModAssignOp:
    case V_SgMultAssignOp:
    case V_SgPlusAssignOp:
    case V_SgPointerAssignOp:
    case V_SgRshiftAssignOp:
    case V_SgXorAssignOp: {
      SgShallowCopy copier;
      SgBinaryOp *op_new = (SgBinaryOp*)op->copy(copier);
      op_new->set_lhs_operand(lhs_attr.completeExpression());
      op_new->set_rhs_operand(rhs_attr.completeExpression());
      result.setNewExpression(op_new);
      break;
    }
    case V_SgArrowExp: {
      result = foldArrowExp(op, lhs_attr, rhs_attr);
      break;
    }
    case V_SgDotExp: {
      SynthesizedAttribute address_attr(attributes);
      if (lhs_attr.popOmitParentAddressOfOp()) {
        address_attr = lhs_attr;
      }
      else {
        address_attr.setNewExpression(SageBuilder::buildAddressOfOp(
          lhs_attr.completeExpression()
        ));
      }
      result = foldArrowExp(op, address_attr, rhs_attr);
      break;
    }
    case V_SgPntrArrRefExp: {
      SgExpression *lhs_expr = lhs_attr.completeExpression();
      SgExpression *rhs_expr = rhs_attr.completeExpression();
      SgAddOp *add_op = SageBuilder::buildAddOp(lhs_expr, rhs_expr);
      result = foldArithmeticBinaryOp(add_op, lhs_attr, rhs_attr);
      delete lhs_expr;
      delete rhs_expr;
      delete add_op;
      result = foldPointerDerefExp(result, op->get_parent());
      break;
    }
    // C++  is not implemented.
    case V_SgArrowStarOp:
    case V_SgScopeOp: // not even used according to comments in ROSE
    case V_SgUserDefinedBinaryOp:
    case V_SgDotStarOp:
    // Fortran is not implemented.
    case V_SgConcatenationOp:
    case V_SgExponentiationOp:
    default: {
      std::cerr << "ConstantFolder: unimplemented binary operator: "
                << op->class_name() << std::endl;
      ROSE2LLVM_ASSERT(false);
      break;
    }
  }
  return result;
}

// Evaluate a conditional expression i.e. a?b:c
ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::foldConditionalExp(
  SynthesizedAttribute c_attr, SynthesizedAttribute t_attr,
  SynthesizedAttribute f_attr
) {
  SgBoolValExp const *expr_c = isSgBoolValExp(
    foldCast(SageBuilder::buildBoolType(), c_attr).getExpression()
  );
  SynthesizedAttribute result(attributes);
  if (!expr_c) {
    result.setNewExpression(SageBuilder::buildConditionalExp(
      c_attr.completeExpression(), t_attr.completeExpression(),
      f_attr.completeExpression()
    ));
  }
  else if (expr_c->get_value()) {
    result = t_attr;
  }
  else {
    result = f_attr;
  }
  return result;
}

ConstantFolder::InheritedAttribute
ConstantFolder::ExpressionTraversal::evaluateInheritedAttribute(
  SgNode *node, InheritedAttribute inherited_attribute
) {
  ConstantFolder::InheritedAttribute result;
  result.inheritFrom(inherited_attribute);
  if (isSgSizeOfOp(node)) {
    result.ignoreChildren();
  }
  return result;
}

ConstantFolder::SynthesizedAttribute
ConstantFolder::ExpressionTraversal::evaluateSynthesizedAttribute(
  SgNode *node,
  InheritedAttribute inherited_attribute,
  SynthesizedAttributesList synthesized_attribute_list
) {
  SynthesizedAttribute result(attributes);
  if (inherited_attribute.isIgnored()) {
    return result;
  }
  SgExpression *expr = isSgExpression(node);
  // This is guaranteed because ExpressionTraversal::traverse only
  // accepts an SgExpression.
  ROSE2LLVM_ASSERT(expr);
  // rose/src/frontend/SageIII/Cxx_GrammarTreeTraversalAccessEnums.h
  // defines the enums for indexing synthesized_attribute_list.  For
  // example:
  //
  //   enum E_SgConditionalExp {
  //     SgConditionalExp_conditional_exp,
  //     SgConditionalExp_true_exp,
  //     SgConditionalExp_false_exp
  //   }
  if (isSgValueExp(expr)) {
    ROSE2LLVM_ASSERT(synthesized_attribute_list.size() <= 1);
    result.setOldExpression(expr);
  }
  else if (isSgVarRefExp(expr)) {
    ROSE2LLVM_ASSERT(synthesized_attribute_list.size() == 0);
    result.setOldExpression(expr);
  }
  else if (isSgUnaryOp(expr)) {
    SgUnaryOp const *op = isSgUnaryOp(expr);
    ROSE2LLVM_ASSERT(
      synthesized_attribute_list.size() == 1
      || (synthesized_attribute_list.size() == 2 && isSgCastExp(op))
    );
    result = foldUnaryOp(
      op, synthesized_attribute_list[SgUnaryOp_operand_i]
    );
  }
  else if (isSgBinaryOp(expr)) {
    SgBinaryOp const *op = isSgBinaryOp(expr);
    ROSE2LLVM_ASSERT(synthesized_attribute_list.size() == 2);
    result = foldBinaryOp(
      op,
      synthesized_attribute_list[SgBinaryOp_lhs_operand_i],
      synthesized_attribute_list[SgBinaryOp_rhs_operand_i]
    );
  }
  // a ? b : c
  else if (isSgConditionalExp(expr)) {
    ROSE2LLVM_ASSERT(synthesized_attribute_list.size() == 3);
    result = foldConditionalExp(
      synthesized_attribute_list[SgConditionalExp_conditional_exp],
      synthesized_attribute_list[SgConditionalExp_true_exp],
      synthesized_attribute_list[SgConditionalExp_false_exp]
    );
  }
  else if (isSgSizeOfOp(expr)) {
    SgSizeOfOp *size_of = isSgSizeOfOp(expr);
    SgType *operand_type = size_of->get_operand_type();
    {
      SgExpression *operand_expr = size_of->get_operand_expr();
      // The sizeof operand is a type or an expression.
      ROSE2LLVM_ASSERT((!operand_type) != (!operand_expr));
      if (operand_expr) {
        operand_type = operand_expr->get_type();
        // For array variables for which the number of elements is
        // computed from the initializer (for example, int a[] = {1,
        // 2};), ROSE computes the type of the array with an unspecified
        // number elements.  If the number of elements is explicitly
        // declared and the initializer has less elements (for example,
        // int a[3] = {1, 2};), ROSE sets the type of the initializer to
        // reflect the full size.  Thus, in all cases, use the
        // initializer's type.
        //
        // There are only two other cases we have found where the
        // unspecified dimension of an array affects a type.  First, the
        // operand of the SgSizeOfOp might be an SgAddressOfOp, whose
        // operand might be an SgVarRefExp for such an array.  While
        // that does affect the type of the expression, it does not
        // affect the size of the expression, which is just a pointer.
        // Second, the operand of an SgSizeOfOp might be in a
        // comma-delimited list.  In that case, ROSE discards all but
        // the last element of such a list, so that case is eliminated
        // before we can see it.
        if (isSgVarRefExp(operand_expr)) {
          SgInitializer *init =
            isSgVarRefExp(operand_expr)->get_symbol()->get_declaration()
              ->get_initializer();
          if (init) {
            operand_type = init->get_type();
          }
        }
      }
    }
    // FIXME: The LLVM_SIZE attribute stores its value in an int.  Might
    // that ever cause the value to be truncated?
    attributes->setLLVMTypeName(operand_type);
    SgValueExp *value;
    // In our tests on x86-64, ROSE always selects SgTypeUnsignedInt
    // as the return type of sizeof in a constant expression.
    // However, we instead use the type that the host compiler
    // selects.  FIXME: And eventually, we want to select it based on
    // the target platform.
    if (false) ;
    #define CF_CASE(T) \
    else if (sizeof(size_t) == sizeof(CF_##T##_HTYPE)) { \
      value = CF_##T##_DBVAL( \
        ((IntAstAttribute*) \
          operand_type->getAttribute(Control::LLVM_SIZE) \
        )->getValue() \
      ); \
    }
    // We can't use a switch on sizeof(size_t) because sizeof is not
    // guaranteed to give different sizes in all these cases and a
    // switch cannot have two cases with the same value.
    CF_CASE(UNSIGNED_INT)
    CF_CASE(UNSIGNED_LONG)
    CF_CASE(UNSIGNED_LONG_LONG)
    #undef CF_CASE
    else {
      std::cerr << "ConstantFolder: size_t has an unexpected size"
                << std::endl;
      ROSE2LLVM_ASSERT(false);
    }
    result.setNewExpression(value);
  }
  // FIXME: There are many other kinds of SgExpression to handle.
  else {
    std::cerr << "ConstantFolder: unimplemented expression type: "
              << expr->class_name() << std::endl;
    ROSE2LLVM_ASSERT(false);
  }
  return result;
}
