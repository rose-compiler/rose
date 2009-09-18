#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "typeLayoutStore.h"
#include <interp_core.h>

using namespace std;
using namespace boost;

namespace {

// From projects/compass/extensions/checkers/dynamicCast/dynamicCast.C
//The function 'isBaseClassOf(...)' returns true if the first paramater is a base
//class of the second paramater.
static bool isBaseClassOf(SgClassDeclaration* base, SgClassDeclaration* clDecl)
   {  
     ROSE_ASSERT(base != NULL);
     ROSE_ASSERT(clDecl != NULL);

     if( clDecl->get_definingDeclaration() == NULL )
          return false;

     SgClassDefinition* clDef = isSgClassDeclaration(clDecl->get_definingDeclaration())->get_definition();

     if(clDef == NULL)
          return false;

     SgBaseClassPtrList&  baseClLst = clDef->get_inheritances();

     for(SgBaseClassPtrList::iterator i = baseClLst.begin();
                     i != baseClLst.end(); ++i )
        {

          SgClassDeclaration* baseCl = (*i)->get_base_class();

          if( base ==  baseCl )
               return true;

          if( isBaseClassOf( baseCl, clDecl ) == true  )
               return true;
        } 

     return false;
   }
 
//! Returns the type of the given field from a StructLayoutEntry
static SgType *fieldType(SgNode *field)
   {
     if (SgInitializedName *in = isSgInitializedName(field))
        {
          return in->get_type();
        }
     else if (SgBaseClass *bc = isSgBaseClass(field))
        {
          return bc->get_base_class()->get_type();
        }
     else if (SgClassDeclaration *cd = isSgClassDeclaration(field))
        {
          return cd->get_type();
        }
     else
        {
          cerr << "fieldType: unrecognised field declaration " << field->sage_class_name() << endl;
          return 0;
        }
   }

}; // anonymous namespace

namespace Interp {

Value *mkBoolean(bool b, Position pos, StackFrameP owner)
   {
     SgFile::outputLanguageOption_enum lang = owner->get_language();
     if (lang == SgFile::e_C_output_language)
        {
          return new IntegralPrimTypeValue<int>(b, pos, owner);
        }
     else if (lang == SgFile::e_Cxx_output_language)
        {
          return new IntegralPrimTypeValue<bool>(b, pos, owner);
        }
     else
        {
          stringstream ss;
          ss << "Unsupported language " << int(lang);
          throw InterpError(ss.str());
        }
   }

Value *mkBoolean(Position pos, StackFrameP owner)
   {
     SgFile::outputLanguageOption_enum lang = owner->get_language();
     if (lang == SgFile::e_C_output_language)
        {
          return new IntegralPrimTypeValue<int>(pos, owner);
        }
     else if (lang == SgFile::e_Cxx_output_language)
        {
          return new IntegralPrimTypeValue<bool>(pos, owner);
        }
     else
        {
          stringstream ss;
          ss << "Unsupported language " << int(lang);
          throw InterpError(ss.str());
        }
   }

InterpError::InterpError(string err) : err(err)
     {
       callStack.push_back(NULL);
     }

void InterpError::dumpCallStack(ostream &out)
   {
     for (size_t i = 0; i < callStack.size(); i++)
        {
          SgStatement *frame = callStack[i];
          out << " at ";
          SgNode *parent = frame;
          SgFunctionDeclaration *decl = NULL;
          while (decl == NULL)
             {
               if (parent == NULL) break;
               parent = parent->get_parent();
               decl = isSgFunctionDeclaration(parent);
             }
          if (decl == NULL)
             {
               out << "<<unknown>>" << endl;
               continue;
             }
          out << decl->get_qualified_name().getString();
          Sg_File_Info *soc = frame->get_startOfConstruct();
          if (soc)
             {
               out << " (" << soc->get_filenameString() << ":" << soc->get_line() << ")";
             }
          out << endl;
        }
   }

FieldVisitor::~FieldVisitor() {}

Value::Value(Position pos, StackFrameP owner, bool valid) : pos(pos), owner(owner), valid(valid)
   {
     if (owner->interp()->trace)
          cout << "Value::Value @" << this << endl;
   }

Value::~Value()
   {
     if (owner->interp()->trace)
          cout << "Value::~Value @" << this << endl;
   }

void Value::destroy()
   {
     valid = false;
   }

ValueP Value::fieldAtOffset(size_t offset)
   {
     if (offset != 0)
        {
          stringstream ss;
          ss << "Attempt to access primitive value at offset " << offset;
          throw InterpError(ss.str());
        }
     return shared_from_this();
   }

const_ValueP Value::fieldAtOffset(size_t offset) const
   {
     if (offset != 0)
        {
          stringstream ss;
          ss << "Attempt to access primitive value at offset " << offset;
          throw InterpError(ss.str());
        }
     return shared_from_this();
   }

ValueP Value::primAtOffset(size_t offset)
   {
     ValueP field = fieldAtOffset(offset);
     ValueP fieldp;
     // As soon as we reach a fixed point, we know the value cannot be broken down any further
     do
        {
          fieldp = field;
          field = field->fieldAtOffset(0);
        }
     while (fieldp != field);
     return field;
   };

const_ValueP Value::primAtOffset(size_t offset) const
   {
     const_ValueP field = fieldAtOffset(offset), fieldp;
     // As soon as we reach a fixed point, we know the value cannot be broken down any further
     do
        {
          fieldp = field;
          field = field->fieldAtOffset(0);
        }
     while (fieldp != field);
     return field;
   };

void Value::forEachField(FieldVisitor &visit)
   {
     visit(0, shared_from_this());
   }

bool Value::hasDynamicTypeAtOffset(SgClassType *t, size_t offset) const
   {
     return false;
   }

size_t Value::backwardValidity() const { return 0; }

ValueP Value::dereference() const
   {
     throw InterpError("Attempt to dereference a value that cannot be dereferenced!");
   }

bool Value::getConcreteValueBool() const
   {
     throw InterpError("This value has no concrete representation of type bool");
   }

char Value::getConcreteValueChar() const
   {
     throw InterpError("This value has no concrete representation of type char");
   }

double Value::getConcreteValueDouble() const
   {
     throw InterpError("This value has no concrete representation of type double");
   }

float Value::getConcreteValueFloat() const
   {
     throw InterpError("This value has no concrete representation of type float");
   }

int Value::getConcreteValueInt() const
   {
     throw InterpError("This value has no concrete representation of type int");
   }

long double Value::getConcreteValueLongDouble() const
   {
     throw InterpError("This value has no concrete representation of type long double");
   }

long int Value::getConcreteValueLong() const
   {
     throw InterpError("This value has no concrete representation of type long int");
   }

long long int Value::getConcreteValueLongLong() const
   {
     throw InterpError("This value has no concrete representation of type long long int");
   }

short Value::getConcreteValueShort() const
   {
     throw InterpError("This value has no concrete representation of type short");
   }

unsigned char Value::getConcreteValueUnsignedChar() const
   {
     throw InterpError("This value has no concrete representation of type unsigned char");
   }

unsigned int Value::getConcreteValueUnsignedInt() const
   {
     throw InterpError("This value has no concrete representation of type unsigned int");
   }

unsigned long long int Value::getConcreteValueUnsignedLongLong() const
   {
     throw InterpError("This value has no concrete representation of type unsigned long long int");
   }

unsigned long Value::getConcreteValueUnsignedLong() const
   {
     throw InterpError("This value has no concrete representation of type unsigned long");
   }

unsigned short Value::getConcreteValueUnsignedShort() const
   {
     throw InterpError("This value has no concrete representation of type unsigned short");
   }

#define GDEFINE_UNDEF_UNOP_FN(op,opname,cv) \
ValueP Value::eval##opname(SgType *apt) cv \
   { \
     throw InterpError("The " #op " operator is not defined for this type of value"); \
   }
#define DEFINE_UNDEF_UNOP_FN(op,opname) GDEFINE_UNDEF_UNOP_FN(op,opname,const)

#define GDEFINE_UNDEF_BINOP_FN(op,opname,cv) \
ValueP Value::eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) cv \
   { \
     throw InterpError("The " #op " operator is not defined for this type of value"); \
   }

#define DEFINE_ASSIGN_BINOP_FN(opname,opassignname) \
ValueP Value::eval##opassignname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) \
  { \
    return assign(eval##opname(rhs, lhsApt, rhsApt), lhsApt, rhsApt); \
  }

#define DEFINE_UNDEF_BINOP_FN(op,opname) GDEFINE_UNDEF_BINOP_FN(op,opname,const)
#define DEFINE_UNDEF_BINOP_FNS(op,opname,opassignname) \
     GDEFINE_UNDEF_BINOP_FN(op,opname,const) \
     DEFINE_ASSIGN_BINOP_FN(opname,opassignname)
#define DEFINE_UNDEF_SC_BINOP_FN(op,opname,sctype) DEFINE_UNDEF_BINOP_FN(op,opname)

FOREACH_UNARY_PRIMOP(          DEFINE_UNDEF_UNOP_FN)
FOREACH_NOFP_UNARY_PRIMOP(     DEFINE_UNDEF_UNOP_FN)
FOREACH_BINARY_PRIMOP(         DEFINE_UNDEF_BINOP_FNS)
FOREACH_NOFP_BINARY_PRIMOP(    DEFINE_UNDEF_BINOP_FNS)
FOREACH_SHIFT_PRIMOP(          DEFINE_UNDEF_BINOP_FNS)
FOREACH_BOOL_UNARY_PRIMOP(     DEFINE_UNDEF_UNOP_FN)
FOREACH_BOOL_BINARY_PRIMOP(    DEFINE_UNDEF_BINOP_FN)

GDEFINE_UNDEF_UNOP_FN(++,PrefixPlusPlusOp,)
GDEFINE_UNDEF_UNOP_FN(++,PostfixPlusPlusOp,)
GDEFINE_UNDEF_UNOP_FN(--,PrefixMinusMinusOp,)
GDEFINE_UNDEF_UNOP_FN(--,PostfixMinusMinusOp,)

#undef GDEFINE_UNDEF_UNOP_FN
#undef DEFINE_UNDEF_UNOP_FN
#undef GDEFINE_UNDEF_BINOP_FN
#undef DEFINE_UNDEF_BINOP_FN
#undef DEFINE_UNDEF_BINOP_FNS

#define GDEFINE_BCV_UNOP_FN(op,opname,cv) \
ValueP BaseCompoundValue::eval##opname(SgType *apt) cv \
   { \
     return prim()->eval##opname(apt); \
   }
#define DEFINE_BCV_UNOP_FN(op,opname) GDEFINE_BCV_UNOP_FN(op,opname,const)

#define GDEFINE_BCV_BINOP_FN(op,opname,cv) \
ValueP BaseCompoundValue::eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) cv \
   { \
     return prim()->eval##opname(rhs, lhsApt, rhsApt); \
   }

#define DEFINE_BCV_BINOP_FN(op,opname) GDEFINE_BCV_BINOP_FN(op,opname,const)
#define DEFINE_BCV_BINOP_FNS(op,opname,opassignname) \
     GDEFINE_BCV_BINOP_FN(op,opname,const) \
     GDEFINE_BCV_BINOP_FN(op,opassignname,)
#define DEFINE_BCV_SC_BINOP_FN(op,opname,sctype) DEFINE_BCV_BINOP_FN(op,opname)

FOREACH_UNARY_PRIMOP(          DEFINE_BCV_UNOP_FN)
FOREACH_NOFP_UNARY_PRIMOP(     DEFINE_BCV_UNOP_FN)
FOREACH_BINARY_PRIMOP(         DEFINE_BCV_BINOP_FNS)
FOREACH_NOFP_BINARY_PRIMOP(    DEFINE_BCV_BINOP_FNS)
FOREACH_SHIFT_PRIMOP(          DEFINE_BCV_BINOP_FNS)
FOREACH_BOOL_UNARY_PRIMOP(     DEFINE_BCV_UNOP_FN)
FOREACH_BOOL_BINARY_PRIMOP(    DEFINE_BCV_BINOP_FN)

#undef GDEFINE_BCV_UNOP_FN
#undef DEFINE_BCV_UNOP_FN
#undef GDEFINE_BCV_BINOP_FN
#undef DEFINE_BCV_BINOP_FN
#undef DEFINE_BCV_BINOP_FNS
#undef DEFINE_BCV_SC_BINOP_FN


#define DEFINE_PV_BOOL_BINOP_FN(op,opname) \
ValueP PointerValue::eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const \
   { \
     const PointerValue *rhsPtr = dynamic_cast<const PointerValue *>(rhs->prim().get()); \
     if ((!isSgPointerType(lhsApt) && !isSgArrayType(lhsApt)) \
      || (!isSgPointerType(rhsApt) && !isSgArrayType(rhsApt))) \
        { \
          throw InterpError("Operator " #op " applied to non-pointer type"); \
        } \
     if (!rhsPtr) \
        { \
          throw InterpError("Operator " #op " between incompatible types"); \
        } \
     if (this->valid && rhsPtr->valid) \
          return ValueP(mkBoolean(this->target.get() op rhsPtr->target.get(), PTemp, this->owner)); \
     else \
          return ValueP(mkBoolean(PTemp, this->owner)); \
   }

FOREACH_BOOL_BINARY_PRIMOP(DEFINE_PV_BOOL_BINOP_FN)

#undef DEFINE_PV_BOOL_BINOP_FN

ValueP PointerValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     const PointerValue *rhsPtr = dynamic_cast<const PointerValue *>(rhs->prim().get());
     if ((!isSgPointerType(lhsApt) && !isSgArrayType(lhsApt) && !isSgTypeString(lhsApt))
      || (!isSgPointerType(rhsApt) && !isSgArrayType(rhsApt) && !isSgTypeString(rhsApt)))
        {
          throw InterpError("Operator = applied to non-pointer type");
        }
     if (!rhsPtr)
        {
          throw InterpError("Operator = between incompatible types");
        }
     if (rhsPtr->valid)
        {
          valid = true;
          target = rhsPtr->target;
        }
     else
          valid = false;
     return shared_from_this();
   }

string PointerValue::show() const
   {
     if (target.get() == NULL)
          return "<<null-ptr>>";
     stringstream ss;
     // TODO: This won't work for circular structures - will need to think of something else
     ss << target << " &{" << target->show() << "}";
     return ss.str();
   }

size_t PointerValue::forwardValidity() const { return sizeof(void*); }

ValueP PointerValue::dereference() const
   {
     if (target.get() == NULL)
        {
          throw InterpError("Attempt to dereference a NULL pointer!");
        }
     return target;
   }

ValueP PointerValue::adjustedOffset(SgType *apt, size_t offset) const
   {
     if (target.get() == NULL)
        {
          throw InterpError("Cannot adjust a NULL pointer!");
        }
     SgType *st = apt->stripTypedefsAndModifiers();
     SgType *baseType = NULL;
     if (SgPointerType *pt = isSgPointerType(st))
          baseType = pt->get_base_type();
     else if (SgArrayType *at = isSgArrayType(st))
          baseType = at->get_base_type();
     ROSE_ASSERT(baseType != NULL);
     const StructLayoutInfo &info = typeLayout(baseType);
     return ValueP(new OffsetValue(target, offset*info.size));
   }

ValueP PointerValue::evalAddOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     const BasePrimTypeValue *rhsPrim = dynamic_cast<const BasePrimTypeValue *>(rhs.get());
     ROSE_ASSERT(rhsPrim != NULL);
     ValueP ofs = adjustedOffset(lhsApt, rhsPrim->getConcreteValueInt());
     return ValueP(new PointerValue(ofs, PTemp, owner));
   }

ValueP PointerValue::evalSubtractOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     const BasePrimTypeValue *rhsPrim = dynamic_cast<const BasePrimTypeValue *>(rhs.get());
     ROSE_ASSERT(rhsPrim != NULL);
     ValueP ofs = adjustedOffset(lhsApt, -rhsPrim->getConcreteValueInt());
     return ValueP(new PointerValue(ofs, PTemp, owner));
   }

ValueP PointerValue::evalPrefixPlusPlusOp(SgType *apt)
   {
     target = adjustedOffset(apt, 1);
     return shared_from_this();
   }

ValueP PointerValue::evalPostfixPlusPlusOp(SgType *apt)
   {
     ValueP result (new PointerValue(target, PTemp, owner));
     target = adjustedOffset(apt, 1);
     return result;
   }

ValueP PointerValue::evalPrefixMinusMinusOp(SgType *apt)
   {
     target = adjustedOffset(apt, -1);
     return shared_from_this();
   }

ValueP PointerValue::evalPostfixMinusMinusOp(SgType *apt)
   {
     ValueP result (new PointerValue(target, PTemp, owner));
     target = adjustedOffset(apt, -1);
     return result;
   }

string SymbolValue::show() const
   {
     return getSymbol()->unparseToString();
   }

ValueP SymbolValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     throw InterpError("Attempt to assign to a SymbolValue");
   }

ValueP Value::evalDotExp(ValueP lhs, SgType *lhsApt) const
   {
     throw InterpError("Operator . not defined for this type of value");
   }


/* The size of a SymbolValue is generally undefined and usually does not make sense but
   some compilers treat a pointer to one as a void* */
size_t SymbolValue::forwardValidity() const { return 1; }

SgSymbol *StaticFunctionValue::getSymbol() const { return symbol; }
SgSymbol *NonstaticFunctionValue::getSymbol() const { return symbol; }
SgSymbol *ThisNonstaticFunctionValue::getSymbol() const { return symbol; }
SgSymbol *NonstaticFieldValue::getSymbol() const { return symbol; }

string ThisNonstaticFunctionValue::show() const
   {
     stringstream ss;
     ss << pThis << "->" << SymbolValue::show();
     return ss.str();
   }

ValueP BaseCompoundValue::dereference() const
   {
     return prim()->dereference();
   }

ValueP BaseCompoundValue::evalPrefixPlusPlusOp(SgType *apt)
   {
     prim()->evalPrefixPlusPlusOp(apt);
     return shared_from_this();
   }

ValueP BaseCompoundValue::evalPostfixPlusPlusOp(SgType *apt)
   {
     return prim()->evalPostfixPlusPlusOp(apt);
   }

ValueP BaseCompoundValue::evalPrefixMinusMinusOp(SgType *apt)
   {
     prim()->evalPrefixMinusMinusOp(apt);
     return shared_from_this();
   }

ValueP BaseCompoundValue::evalPostfixMinusMinusOp(SgType *apt)
   {
     return prim()->evalPostfixMinusMinusOp(apt);
   }

OffsetValue::OffsetValue(ValueP base, size_t offset) : BaseCompoundValue(base->pos, base->owner, base->valid)
   {
     if (const OffsetValue *baseOfs = dynamic_cast<const OffsetValue *>(base.get()))
        {
          this->base = baseOfs->base;
          this->offset = offset + baseOfs->offset;
        }
     else
        {
          this->base = base;
          this->offset = offset;
        }
   }

size_t OffsetValue::forwardValidity() const
   {
     return base->forwardValidity() - offset;
   }

size_t OffsetValue::backwardValidity() const
   {
     return base->backwardValidity() + offset;
   }

string OffsetValue::show() const
   {
     stringstream ss;
     ss << "@" << offset << "{" << base->show() << "}";
     return ss.str();
   }

ValueP OffsetValue::fieldAtOffset(size_t offset)
   {
     return base->fieldAtOffset(this->offset + offset);
   }

const_ValueP OffsetValue::fieldAtOffset(size_t offset) const
   {
     return base->fieldAtOffset(this->offset + offset);
   }

std::string CompoundValue::show() const
   {
     // TODO: use the dynamicTypes information to show field names if possible
     stringstream ss;
     ss << "{";
     for (map<size_t, ValueP>::const_iterator fieldI = fields.begin(); fieldI != fields.end(); ++fieldI)
        {
          if (fieldI != fields.begin()) ss << ", ";
          ss << "@" << fieldI->first << " => " << fieldI->second->show();
        }
     ss << "}";
     return ss.str();
   }

ValueP Value::assign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     /*
     struct AssignField : PrimVisitor
        {
          void operator()(size_t offset, ValueP value)
             {
               prims[offset]->assign(value, 0, 0); // TODO: use rhsApt to extract primitive types at offsets
             }
        };
     AssignField af;
     forEachPrim(af);
     */
     if (isSgClassType(rhsApt))
        {
       // TODO: call operator= if extant
          const StructLayoutInfo &info = typeLayout(rhsApt);
          for (std::vector<StructLayoutEntry>::const_iterator fieldI = info.fields.begin(); fieldI != info.fields.end(); ++fieldI)
             {
               SgType *fldType = fieldType(fieldI->decl);
               ValueP lhsOfs(new OffsetValue(shared_from_this(), fieldI->byteOffset));
               ValueP rhsOfs(new OffsetValue(const_pointer_cast<Value, const Value>(rhs), fieldI->byteOffset));
               lhsOfs->assign(rhsOfs, fldType, fldType);
             }
          return shared_from_this();
        }
     else
        {
          owner->interp()->prePrimAssign(shared_from_this(), rhs);
          return primAssign(rhs, lhsApt, rhsApt);
        }
   }

void CompoundValue::forEachField(FieldVisitor &visit)
   {
     for (fieldMap_t::const_iterator fieldI = fields.begin(); fieldI != fields.end(); ++fieldI)
        {
          visit(fieldI->first, fieldI->second);
        }
   }

ValueP Value::copy(SgType *apt, Position pos, StackFrameP owner, bool isParam) const
   {
     ValueP newVal = owner->newValue(apt, pos, isParam);
     return newVal->assign(this->shared_from_this(), apt, apt);
     if (SgClassType *ct = isSgClassType(apt))
        {
       // TODO: call copy-ctor if extant
          const StructLayoutInfo &info = typeLayout(apt);
          CompoundValue::fieldMap_t fields;
          for (std::vector<StructLayoutEntry>::const_iterator fieldI = info.fields.begin(); fieldI != info.fields.end(); ++fieldI)
             {
               SgType *fldType = fieldType(fieldI->decl);
               ValueP rhsOfs (new OffsetValue(const_pointer_cast<Value, const Value>(shared_from_this()), fieldI->byteOffset));
               fields[fieldI->byteOffset] = rhsOfs->copy(fldType, pos, owner);
             }
          return ValueP(new CompoundValue(ct, fields, pos, owner));
        }
     else
        {
          return primCopy(apt, pos, owner, isParam);
        }
   }

ValueP Value::primCopy(SgType *apt, Position pos, StackFrameP owner, bool isParam) const
   {
     ValueP newVal = owner->newValue(apt, pos);
     return newVal->primAssign(this->shared_from_this(), apt, apt);
   }

SgFunctionSymbol *Value::getStaticFunctionSymbol() const
   {
     throw InterpError("Value is not a static function reference");
   }

void Value::setStaticFunctionSymbol(SgFunctionSymbol *sym)
   {
     throw InterpError("Value is not a static function reference");
   }

SgMemberFunctionSymbol *Value::getNonstaticFunctionSymbol() const
   {
     throw InterpError("Value is not a non-static function reference");
   }

void Value::setNonstaticFunctionSymbol(SgMemberFunctionSymbol *sym)
   {
     throw InterpError("Value is not a non-static function reference");
   }

StackFrameP Value::createStackFrame() const
   {
     throw InterpError("Attempt to create a stack frame for a non-function object!");
   }

SgFunctionSymbol *StaticFunctionValue::getStaticFunctionSymbol() const
   {
     return symbol;
   }

void StaticFunctionValue::setStaticFunctionSymbol(SgFunctionSymbol *sym)
   {
     symbol = sym;
   }

SgMemberFunctionSymbol *NonstaticFunctionValue::getNonstaticFunctionSymbol() const
   {
     return symbol;
   }

void NonstaticFunctionValue::setNonstaticFunctionSymbol(SgMemberFunctionSymbol *sym)
   {
     symbol = sym;
   }

StackFrameP StaticFunctionValue::createStackFrame() const
   {
     return owner->newStackFrame(symbol);
   }

StackFrameP ThisNonstaticFunctionValue::createStackFrame() const
   {
     return owner->newStackFrame(symbol, pThis);
   }

CompoundValue::CompoundValue(SgClassType *dynamicType, Position pos, StackFrameP owner) : BaseCompoundValue(pos, owner, true), dynamicType(dynamicType)
     {
       const StructLayoutInfo &info = typeLayout(dynamicType);
       size = info.size;
       for (std::vector<StructLayoutEntry>::const_iterator fieldI = info.fields.begin(); fieldI != info.fields.end(); ++fieldI)
          {
            SgType *fldType = fieldType(fieldI->decl);
            fields[fieldI->byteOffset] = owner->newValue(fldType, pos);
          }
     }

CompoundValue::CompoundValue(SgClassType *dynamicType, fieldMap_t fields, Position pos, StackFrameP owner, size_t size) : 
        BaseCompoundValue(pos, owner, true), 
        fields(fields), 
        size(size == SIZE_MAX ? typeLayout(dynamicType).size : size), 
        dynamicType(dynamicType) {}

/*
ValueP CompoundValue::copy() const
   {

     // TODO: call copy ctor if extant
     for (map<size_t, ValueP>::const_iterator primI = prims.begin(); primI != prims.end(); ++primI)
        {
          ValueP newVal = primI->second->another();
          newVal->copy(primI->second);
          prims[primI->first] = newVal;
        }
   }


 
ValueP CompoundValue::copy(const_ValueP rhs);
*/

void CompoundValue::destroy()
   {
     // TODO: call the destructor
     valid = false;
     for (fieldMap_t::iterator fieldI = fields.begin(); fieldI != fields.end(); ++fieldI)
        {
          fieldI->second->destroy();
        }
   }

ValueP CompoundValue::fieldAtOffset(size_t offset)
   {
     if (offset < 0 || offset >= size)
        {
          throw InterpError("Attempt to access a field out of range");
        }
     fieldMap_t::const_iterator fieldPos = fields.upper_bound(offset);
     if (fieldPos == fields.begin())
        {
          throw InterpError("Attempt to access a field in a badly constructed compound");
        }
     fieldPos--;
     if (fieldPos->first == offset)
          return fieldPos->second;
     else
          return fieldPos->second->fieldAtOffset(offset - fieldPos->first);
   }

const_ValueP CompoundValue::fieldAtOffset(size_t offset) const
   {
     return const_cast<CompoundValue*>(this)->fieldAtOffset(offset);
   }

ValueP BaseCompoundValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     prim()->primAssign(rhs, lhsApt, rhsApt);
     return shared_from_this();
   }
vector<SgClassType *> Value::dynamicTypesAtOffset(size_t offset) const
   {
     vector<SgClassType *> dynTypes;
     buildDynamicTypesAtOffset(offset, dynTypes);
     return dynTypes;
   }

void Value::buildDynamicTypesAtOffset(size_t offset, vector<SgClassType *> &types) const {}

void CompoundValue::buildDynamicTypesAtOffset(size_t offset, vector<SgClassType *> &types) const
   {
     if (offset == 0) types.push_back(dynamicType);
     fieldAtOffset(offset)->buildDynamicTypesAtOffset(0, types);
   }

bool CompoundValue::hasDynamicTypeAtOffset(SgClassType *t, size_t offset) const
   {
     if (offset == 0 && isBaseClassOf(isSgClassDeclaration(t->get_declaration()),
                                      isSgClassDeclaration(dynamicType->get_declaration())))
          return true;

     return fieldAtOffset(offset)->hasDynamicTypeAtOffset(t, 0);
   }

size_t CompoundValue::forwardValidity() const { return size; }

void dumpBindings(std::ostream &out, const varBindings_t &bindings)
   {
     out << "{";
     for (varBindings_t::const_iterator bindI = bindings.begin(); bindI != bindings.end(); ++bindI)
        {
          if (bindI != bindings.begin()) out << ", ";
          out << bindI->first->get_name().getString() << " => " << bindI->second->show();
        }
     out << "}";
   }

struct StackFrame::BlockStackFrame : boost::enable_shared_from_this<BlockStackFrame>
   {
     BlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgScopeStatement *scope) : up(up), sf(sf), scope(scope) {}
     BlockStackFrameP up;
     StackFrameP sf;
     SgScopeStatement *scope;
     blockScopeVars_t scopeVars;
     virtual SgStatement *next() { return NULL; }

     virtual void destroyScope()
        {
          sf->destroyScope(scopeVars);
        }

     virtual BlockStackFrameP doContinue()
        {
          destroyScope();
          ROSE_ASSERT(up.get() != NULL);
          return up->doContinue();
        }

     virtual BlockStackFrameP doBreak()
        {
          destroyScope();
          ROSE_ASSERT(up.get() != NULL);
          return up->doBreak();
        }

     virtual ~BlockStackFrame() {}
   };

ArrayValue::ArrayValue(SgType *baseType, size_t index, Position pos, StackFrameP owner)
        : PointerValue(ValueP(new CompoundValue(baseType, index, pos, owner)), pos, owner),
          extent(index) {}

ArrayValue::ArrayValue(SgType *baseType, size_t index, CompoundValue::fieldMap_t fields, Position pos, StackFrameP owner)
        : PointerValue(ValueP(new CompoundValue(0, fields, pos, owner, index * typeLayout(baseType).size)), pos, owner),
          extent(index) {}


ValueP ArrayValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     // As far as I am aware this will only happen if a struct containing an array
     // performs a default assignment.  This is because arrays are nonmodifiable
     // (see C++ standard section 8.3.4 para 5). In this case, we should perform a
     // deep assignment.
     SgArrayType *lhsArr = isSgArrayType(lhsApt->stripTypedefsAndModifiers());
     ROSE_ASSERT(lhsArr != NULL);
     SgType *baseType = lhsArr->get_base_type();
     const StructLayoutInfo &info = typeLayout(baseType);
     for (size_t i = 0; i < extent; ++i)
        {
          size_t ofs = i * info.size;
          ValueP lhsElem (new OffsetValue(target, ofs));
          ValueP rhsElem (new OffsetValue(rhs->dereference(), ofs));
          lhsElem->assign(rhsElem, baseType, baseType);
        }
     return shared_from_this();
   }

ValueP ArrayValue::primCopy(SgType *apt, Position pos, StackFrameP owner, bool isParam) const
   {
     // The rules here are:
     // 1. If we are doing parameter passing, we need to pass by reference,
     //    i.e. create a PointerValue
     // 2. If apt is an SgPointerType, this is an implicit array-to-pointer conversion
     //    (C++ standard, section 4.2), and we also need to create a PointerValue
     // 3. Otherwise, we are not doing parameter passing, and this must be a copy performed
     //    by the implicit copy constructor.  Copy by value.
     SgType *st = apt->stripTypedefsAndModifiers();
     if (isParam || isSgPointerType(st))
        {
          return ValueP(new PointerValue(target, pos, owner));
        }
     SgArrayType *at = isSgArrayType(st);
     ROSE_ASSERT(at != NULL);
     SgType *baseType = at->get_base_type();
     const StructLayoutInfo &info = typeLayout(baseType);
     CompoundValue::fieldMap_t fields;
     for (size_t i = 0; i < extent; ++i)
        {
          size_t ofs = i * info.size;
          ValueP rhsElem (new OffsetValue(target, ofs));
          fields[ofs] = rhsElem->copy(baseType, pos, owner);
        }
     return ValueP(new ArrayValue(at, extent, fields, pos, owner));
   }

string ArrayValue::show() const
   {
     stringstream ss;
     ss << "array " << target->show();
     return ss.str();
   }

void ArrayValue::destroy()
   {
     PointerValue::destroy();
     target->destroy();
   }

CompoundValue::CompoundValue(SgType *baseType, size_t index, Position pos, StackFrameP owner) : BaseCompoundValue(pos, owner, true)
   {
     const StructLayoutInfo &info = typeLayout(baseType);
     size = index * info.size;
     for (size_t i = 0; i < index; ++i)
        {
          fields[i * info.size] = owner->newValue(baseType, pos);
        }
   }

ValueP StackFrame::newArray(SgArrayType *at, Position pos)
   {
     SgExpression *index = at->get_index();
     if (index == NULL) /* this is an incomplete type, so it should be represented as a pointer */
          return ValueP(new PointerValue(pos, shared_from_this()));
     ValueP indexVal = evalExpr(index);
     return ValueP(new ArrayValue(at->get_base_type(), indexVal->getConcreteValueInt(), pos, shared_from_this()));
   }

ValueP StackFrame::newValue(SgType *t, Position pos, bool isParam)
   {
     t = t->stripTypedefsAndModifiers();
     switch (t->variantT())
        {
          case V_SgTypeBool: return ValueP(new BoolValue(pos, shared_from_this()));
          case V_SgTypeChar: return ValueP(new CharValue(pos, shared_from_this()));
          case V_SgTypeDouble: return ValueP(new DoubleValue(pos, shared_from_this()));
          case V_SgTypeFloat: return ValueP(new FloatValue(pos, shared_from_this()));
          case V_SgTypeInt: return ValueP(new IntValue(pos, shared_from_this()));
          case V_SgTypeLongDouble: return ValueP(new LongDoubleValue(pos, shared_from_this()));
          case V_SgTypeLong: return ValueP(new LongIntValue(pos, shared_from_this()));
          case V_SgTypeLongLong: return ValueP(new LongLongIntValue(pos, shared_from_this()));
          case V_SgTypeShort: return ValueP(new ShortValue(pos, shared_from_this()));
//        case V_SgTypeString: return ValueP(new StringValue(pos, shared_from_this()));
          case V_SgTypeUnsignedChar: return ValueP(new UnsignedCharValue(pos, shared_from_this()));
          case V_SgTypeUnsignedInt: return ValueP(new UnsignedIntValue(pos, shared_from_this()));
          case V_SgTypeUnsignedLongLong: return ValueP(new UnsignedLongLongIntValue(pos, shared_from_this()));
          case V_SgTypeUnsignedLong: return ValueP(new UnsignedLongValue(pos, shared_from_this()));
          case V_SgTypeUnsignedShort: return ValueP(new UnsignedShortValue(pos, shared_from_this()));
          case V_SgClassType: return ValueP(new CompoundValue(isSgClassType(t), pos, shared_from_this()));
          case V_SgArrayType: if (!isParam) return newArray(isSgArrayType(t), pos);
                                   /* else fallthrough */
          case V_SgPointerType: return ValueP(new PointerValue(pos, shared_from_this()));
          default: throw InterpError("Unable to create a new value of type " + t->class_name());
        }
   }

StackFrameP StackFrame::newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding)
   {
     return StackFrameP(new StackFrame(currentInterp, funSym, thisBinding));
   }

bool StackFrame::isGlobalVar(SgInitializedName *var)
   {
     SgScopeStatement *varScope = var->get_scope();
     return isSgGlobal(varScope) || isSgNamespaceDefinitionStatement(varScope);
   }

ValueP StackFrame::evalVarRefExp(SgVarRefExp *vr)
   {
     SgVariableSymbol *sym = vr->get_symbol();
     SgInitializedName *in = sym->get_declaration();
     if (isSgClassDefinition(in->get_scope()) && !in->get_storageModifier().isStatic())
        {
       // a non-static member variable
          return ValueP(new NonstaticFieldValue(sym, PTemp, shared_from_this()));
        }
     else
        {
       // static, local or global variable
          varBindings_t::const_iterator vari = localVarBindings.find(sym);
          if (vari != localVarBindings.end()) return vari->second;
          varBindings_t &globalVarBindings = currentInterp->globalVarBindings;
          vari = globalVarBindings.find(sym);
          if (vari != globalVarBindings.end()) return vari->second;
          bool isGlobal = isGlobalVar(sym->get_declaration());
          varBindings_t &thisVarBindings = isGlobal ? globalVarBindings : localVarBindings;
          ValueP newVal = newValue(sym->get_type(), isGlobal ? PGlob : PStack);
          thisVarBindings[sym] = newVal;
          return newVal;
        }
   }

ValueP StackFrame::evalFunctionRefExp(SgFunctionSymbol *sym)
   {
  // Note: this should really be a shared PGlobal, so that pointer comparisons etc will work
     return ValueP(new StaticFunctionValue(sym, PTemp, shared_from_this()));
   }

ValueP StackFrame::evalMemberFunctionRefExp(SgMemberFunctionSymbol *sym)
   {
     SgMemberFunctionDeclaration *decl = sym->get_declaration();
     if (decl->get_declarationModifier().get_storageModifier().isStatic())
          return evalFunctionRefExp(sym);
     return ValueP(new NonstaticFunctionValue(sym, PTemp, shared_from_this()));
   }

ValueP StackFrame::evalPointerDerefExp(SgExpression *opd)
   {
     return evalExpr(opd)->dereference();
   }

ValueP StackFrame::evalAddressOfOp(SgExpression *opd)
   {
     return ValueP(new PointerValue(evalExpr(opd), PTemp, shared_from_this()));
   }

ValueP StackFrame::evalCastExp(SgExpression *opd, SgType *toType)
   {
     ValueP from = evalExpr(opd);
     SgType *fromType = opd->get_type();
     ValueP to = newValue(toType, PTemp);
     return to->evalCastExp(from, fromType, toType);
   }

ValueP Value::evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType)
   {
     cout << "TODO: cast " << fromType->unparseToString() << " to " << toType->unparseToString() << endl;
     return fromVal;
   }

ValueP PointerValue::evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType)
   {
     if (isSgPointerType(toType) && isSgTypeInt(fromType))
        {
          if (fromVal->prim()->getConcreteValueInt() != 0)
             {
               throw InterpError("Attempt to cast a non-zero integer to a pointer");
             }
          setTarget(ValueP());
          return shared_from_this();
        }
     else
        {
          return Value::evalCastExp(fromVal, fromType, toType);
        }
   }

ValueP StackFrame::evalAssignOp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalExpr(lhs)->assign(evalExpr(rhs), lhs->get_type(), rhs->get_type());
   }

static size_t sageOffsetOf(ValueP val, SgClassType *apt, SgVariableSymbol *field)
   {
     if (val)
        {
          vector<SgClassType *> dynTypes = val->dynamicTypesAtOffset(0);
          for (vector<SgClassType *>::iterator dynTypeI = dynTypes.begin(); dynTypeI != dynTypes.end(); ++dynTypeI)
             {
               if (isBaseClassOf(isSgClassDeclaration(apt->get_declaration()), isSgClassDeclaration((*dynTypeI)->get_declaration())))
                  {
                    apt = *dynTypeI;
                    break;
                  }
             }
        }
     const StructLayoutInfo &info = typeLayout(apt);
     SgInitializedName *fieldDecl = field->get_declaration();
     for (std::vector<StructLayoutEntry>::const_iterator fieldI = info.fields.begin(); fieldI != info.fields.end(); ++fieldI)
        {
          if (fieldI->decl == fieldDecl)
             {
               return fieldI->byteOffset;
             }
          else if (SgBaseClass *clsBase = isSgBaseClass(fieldI->decl))
             {
               size_t baseOfs = sageOffsetOf(ValueP(), clsBase->get_base_class()->get_type(), field);
               if (baseOfs != SIZE_MAX)
                  {
                    return fieldI->byteOffset + baseOfs;
                  }
             }
        }
     return SIZE_MAX;
   }

ValueP StackFrame::evalFieldOrMethodAccess(ValueP lhsEval, SgClassType *lhsType, SgExpression *rhs)
   {
     // TODO: FunctionRefs for method calls
     SgVarRefExp *rhsVarRef = isSgVarRefExp(rhs);
     ROSE_ASSERT(rhsVarRef != NULL);
     size_t offset = sageOffsetOf(lhsEval, lhsType, rhsVarRef->get_symbol());
     return ValueP(new OffsetValue(lhsEval, offset));
   }

ValueP NonstaticFunctionValue::evalDotExp(ValueP lhs, SgType *lhsApt) const
   {
     return ValueP(new ThisNonstaticFunctionValue(lhs, lhsApt, symbol, PTemp, owner));
   }

ValueP NonstaticFieldValue::evalDotExp(ValueP lhs, SgType *lhsApt) const
   {
     SgClassType *lhsAptCls = isSgClassType(lhsApt->stripTypedefsAndModifiers());
     ROSE_ASSERT(lhsAptCls != NULL);
     size_t offset = sageOffsetOf(lhs, lhsAptCls, symbol);
     return ValueP(new OffsetValue(lhs, offset));
   }

ValueP StackFrame::evalDotExp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalExpr(rhs)->evalDotExp(evalExpr(lhs), lhs->get_type());
   }

ValueP StackFrame::evalArrowExp(SgExpression *lhs, SgExpression *rhs)
   {
     SgPointerType *lhsPtrType = isSgPointerType(lhs->get_type()->stripTypedefsAndModifiers());
     ROSE_ASSERT(lhsPtrType != NULL);
     return evalExpr(rhs)->evalDotExp(evalExpr(lhs)->dereference(), lhsPtrType->get_base_type());
   }

ValueP StackFrame::evalPntrArrRefExp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalAddOp(lhs, rhs)->dereference();
   }

ValueP StackFrame::evalFunctionCallExp(SgFunctionCallExp *fnCall)
   {
     SgExpression *fn = fnCall->get_function();
     ValueP fnVal = evalExpr(fn);
     SgExprListExp *args = fnCall->get_args();
     vector<ValueP> argVals = evalExprListExp(args);
     try
        {
          return fnVal->call(isSgFunctionType(fn->get_type()->stripTypedefsAndModifiers()), argVals);
        }
     catch (InterpError &ie)
        {
          ie.callStack.push_back(NULL);
          throw;
        }
   }

ValueP Value::call(SgFunctionType *fnType, const vector<ValueP> &argVals) const
   {
     StackFrameP down = createStackFrame();
     return down->interpFunction(argVals);
   }

vector<ValueP> StackFrame::evalExprListExp(SgExprListExp *exprList)
   {
     const SgExpressionPtrList &exprs = exprList->get_expressions();
     vector<ValueP> exprVals;
     for (SgExpressionPtrList::const_iterator exprI = exprs.begin(); exprI != exprs.end(); ++exprI)
        {
          exprVals.push_back(evalExpr(*exprI));
        }
     return exprVals;
   }

ValueP StackFrame::evalThisExp()
   {
     return ValueP(new PointerValue(thisBinding, PTemp, shared_from_this()));
   }

ValueP StackFrame::evalPlusPlusOp(SgExpression *opd, SgUnaryOp::Sgop_mode mode)
   {
     ValueP opdVal = evalExpr(opd);
     if (mode == SgUnaryOp::prefix)
        {
          return opdVal->evalPrefixPlusPlusOp(opd->get_type());
        }
     else // postfix
        {
          return opdVal->evalPostfixPlusPlusOp(opd->get_type());
        }
   }

ValueP StackFrame::evalMinusMinusOp(SgExpression *opd, SgUnaryOp::Sgop_mode mode)
   {
     ValueP opdVal = evalExpr(opd);
     if (mode == SgUnaryOp::prefix)
        {
          return opdVal->evalPrefixMinusMinusOp(opd->get_type());
        }
     else // postfix
        {
          return opdVal->evalPostfixMinusMinusOp(opd->get_type());
        }
   }

#define DEFINE_STACKFRAME_EVALUNOP(op,opname) \
ValueP StackFrame::eval##opname(SgExpression *opd) \
   { \
     return evalExpr(opd)->eval##opname(opd->get_type()); \
   }

#define DEFINE_STACKFRAME_EVALBINOP(op,opname) \
ValueP StackFrame::eval##opname(SgExpression *lhs, SgExpression *rhs) \
   { \
     return evalExpr(lhs)->eval##opname(evalExpr(rhs), lhs->get_type(), rhs->get_type()); \
   }

#define DEFINE_STACKFRAME_SC_EVALBINOP(op,opname,sctype) \
ValueP StackFrame::eval##opname(SgExpression *lhs, SgExpression *rhs) \
   { \
     ValueP lhsEval = evalExpr(lhs); \
     BasePrimTypeValue *lhsPrim = dynamic_cast<BasePrimTypeValue *>(lhsEval->prim().get()); \
     if (!lhsPrim) \
        { \
          throw InterpError("LHS of operator " #op " returned value of incorrect type (must be bool or int)"); \
        } \
     if (!lhsPrim->valid || lhsPrim->getConcreteValueInt() == sctype) return lhsEval; \
     return evalExpr(rhs); \
   }

#define DEFINE_STACKFRAME_EVALBINOPS(op,opname,opassignname) \
        DEFINE_STACKFRAME_EVALBINOP(op,opname) \
        DEFINE_STACKFRAME_EVALBINOP(op,opassignname)

FOREACH_UNARY_PRIMOP(          DEFINE_STACKFRAME_EVALUNOP)
FOREACH_NOFP_UNARY_PRIMOP(     DEFINE_STACKFRAME_EVALUNOP)
FOREACH_BOOL_UNARY_PRIMOP(     DEFINE_STACKFRAME_EVALUNOP)
FOREACH_BINARY_PRIMOP(         DEFINE_STACKFRAME_EVALBINOPS)
FOREACH_NOFP_BINARY_PRIMOP(    DEFINE_STACKFRAME_EVALBINOPS)
FOREACH_SHIFT_PRIMOP(          DEFINE_STACKFRAME_EVALBINOPS)
FOREACH_BOOL_BINARY_PRIMOP(    DEFINE_STACKFRAME_EVALBINOP)
FOREACH_BOOL_SC_BINARY_PRIMOP( DEFINE_STACKFRAME_SC_EVALBINOP)

#undef DEFINE_STACKFRAME_EVALBINOPS
#undef DEFINE_STACKFRAME_EVALBINOP

ValueP StackFrame::evalUnaryOp(SgUnaryOp *unOp)
   {
     SgExpression *opd = unOp->get_operand();
     switch (unOp->variantT())
        {
#define STACKFRAME_EVALUNARYOP_CASE(op,opname) \
          case V_Sg##opname: return eval##opname(opd);
          FOREACH_UNARY_PRIMOP(         STACKFRAME_EVALUNARYOP_CASE)
          FOREACH_NOFP_UNARY_PRIMOP(    STACKFRAME_EVALUNARYOP_CASE)
          FOREACH_BOOL_UNARY_PRIMOP(    STACKFRAME_EVALUNARYOP_CASE)
#undef STACKFRAME_EVALUNARYOP_CASE
          case V_SgPointerDerefExp: return evalPointerDerefExp(opd);
          case V_SgAddressOfOp: return evalAddressOfOp(opd);
          case V_SgCastExp: return evalCastExp(opd, unOp->get_type());
          case V_SgPlusPlusOp: return evalPlusPlusOp(opd, unOp->get_mode());
          case V_SgMinusMinusOp: return evalMinusMinusOp(opd, unOp->get_mode());
          default: throw InterpError("unhandled unary operator " + unOp->class_name() + " encountered");
        }
   }

ValueP StackFrame::evalBinaryOp(SgBinaryOp *binOp)
   {
     SgExpression *lhs = binOp->get_lhs_operand(), *rhs = binOp->get_rhs_operand();
     switch (binOp->variantT())
        {
          case V_SgAssignOp: return evalAssignOp(lhs, rhs);
#define STACKFRAME_EVALBINARYOP_CASE(op,opname) \
          case V_Sg##opname: return eval##opname(lhs, rhs);
#define STACKFRAME_SC_EVALBINARYOP_CASE(op,opname,sctype) \
        STACKFRAME_EVALBINARYOP_CASE(op,opname)
#define STACKFRAME_EVALBINARYOP_CASES(op,opname,opassignname) \
        STACKFRAME_EVALBINARYOP_CASE(op,opname) \
        STACKFRAME_EVALBINARYOP_CASE(op,opassignname)
          FOREACH_BINARY_PRIMOP(         STACKFRAME_EVALBINARYOP_CASES)
          FOREACH_NOFP_BINARY_PRIMOP(    STACKFRAME_EVALBINARYOP_CASES)
          FOREACH_SHIFT_PRIMOP(          STACKFRAME_EVALBINARYOP_CASES)
          FOREACH_BOOL_BINARY_PRIMOP(    STACKFRAME_EVALBINARYOP_CASE)
          FOREACH_BOOL_SC_BINARY_PRIMOP( STACKFRAME_SC_EVALBINARYOP_CASE)
#undef STACKFRAME_EVALBINARYOP_CASE
#undef STACKFRAME_SC_EVALBINARYOP_CASE
#undef STACKFRAME_EVALBINARYOP_CASES
          case V_SgDotExp: return evalDotExp(lhs, rhs);
          case V_SgArrowExp: return evalArrowExp(lhs, rhs);
          case V_SgPntrArrRefExp: return evalPntrArrRefExp(lhs, rhs);
          default: throw InterpError("unhandled binary operator " + binOp->class_name() + " encountered");
        }
   }

ValueP StackFrame::stringToValue(const std::string &str)
   {
     SgTypeChar *charType = SgTypeChar::createType();
     ValueP strArray(new CompoundValue(charType, str.size()+1, PGlob, shared_from_this()));
     for (size_t i = 0; i < str.size(); ++i)
        {
          ValueP chr = strArray->primAtOffset(i);
          ValueP chrRhs (new CharValue(str[i], PTemp, shared_from_this()));
          chr->assign(chrRhs, charType, charType);
        }
     ValueP chrZero (new CharValue(0, PTemp, shared_from_this()));
     strArray->primAtOffset(str.size())->assign(chrZero, charType, charType);
     ValueP strPtr (new PointerValue(strArray, PTemp, shared_from_this()));
     return strPtr;
   }

ValueP StackFrame::evalStringVal(SgStringVal *strVal)
   {
     // TODO: store strArray as an attribute of the SgStringVal
     return stringToValue(strVal->get_value());
   }

ValueP StackFrame::evalExpr(SgExpression *expr)
   {
     if (SgUnaryOp *unOp = isSgUnaryOp(expr)) return evalUnaryOp(unOp);
     if (SgBinaryOp *binOp = isSgBinaryOp(expr)) return evalBinaryOp(binOp);
     switch (expr->variantT())
        {
          case V_SgBoolValExp: return evalPrimExpr<BoolValue, SgBoolValExp>(expr);
          case V_SgCharVal: return evalPrimExpr<CharValue, SgCharVal>(expr);
          case V_SgDoubleVal: return evalPrimExpr<DoubleValue, SgDoubleVal>(expr);
          case V_SgFloatVal: return evalPrimExpr<FloatValue, SgFloatVal>(expr);
          case V_SgIntVal: return evalPrimExpr<IntValue, SgIntVal>(expr);
          case V_SgLongDoubleVal: return evalPrimExpr<LongDoubleValue, SgLongDoubleVal>(expr);
          case V_SgLongIntVal: return evalPrimExpr<LongIntValue, SgLongIntVal>(expr);
          case V_SgLongLongIntVal: return evalPrimExpr<LongLongIntValue, SgLongLongIntVal>(expr);
          case V_SgShortVal: return evalPrimExpr<ShortValue, SgShortVal>(expr);
          case V_SgStringVal: return evalStringVal(isSgStringVal(expr));
          case V_SgUnsignedCharVal: return evalPrimExpr<UnsignedCharValue, SgUnsignedCharVal>(expr);
          case V_SgUnsignedIntVal: return evalPrimExpr<UnsignedIntValue, SgUnsignedIntVal>(expr);
          case V_SgUnsignedLongLongIntVal: return evalPrimExpr<UnsignedLongLongIntValue, SgUnsignedLongLongIntVal>(expr);
          case V_SgUnsignedLongVal: return evalPrimExpr<UnsignedLongValue, SgUnsignedLongVal>(expr);
          case V_SgUnsignedShortVal: return evalPrimExpr<UnsignedShortValue, SgUnsignedShortVal>(expr);
          case V_SgVarRefExp: return evalVarRefExp(isSgVarRefExp(expr));
          case V_SgFunctionRefExp: return evalFunctionRefExp(isSgFunctionRefExp(expr)->get_symbol());
          case V_SgMemberFunctionRefExp: return evalMemberFunctionRefExp(isSgMemberFunctionRefExp(expr)->get_symbol());
          case V_SgFunctionCallExp: return evalFunctionCallExp(isSgFunctionCallExp(expr));
          case V_SgThisExp: return evalThisExp();
          default: throw InterpError("unhandled expression " + expr->class_name() + " encountered");
        }
   }

void StackFrame::destroyScope(blockScopeVars_t &scope)
   {
     bool trace = interp()->trace;
     if (trace)
          cout << "destroyScope";
     for (blockScopeVars_t::iterator scopeI = scope.begin(); scopeI != scope.end(); ++scopeI)
        {
          SgVariableSymbol *varSym = *scopeI;
          if (trace)
               cout << " " << varSym->get_name().getString();
          varBindings_t::iterator bindI = localVarBindings.find(varSym);
          ROSE_ASSERT(bindI != localVarBindings.end());
          bindI->second->destroy();
          localVarBindings.erase(bindI);
        }
     if (trace)
          cout << endl;
     scope.clear();
   }

struct StackFrame::BasicBlockStackFrame : BlockStackFrame
     {
       BasicBlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgBasicBlock *bb) : BlockStackFrame(up, sf, bb), stmts(bb->get_statements()), stmtI(stmts.begin()) {}

       SgStatementPtrList &stmts;
       SgStatementPtrList::iterator stmtI;

       SgStatement *next()
          {
            if (stmtI == stmts.end()) return NULL;
            SgStatement *s = *stmtI;
            ++stmtI;
            return s;
          }

     };

void StackFrame::evalBasicBlock(SgBasicBlock *bb, BlockStackFrameP &curFrame)
   {
     curFrame = BlockStackFrameP(new BasicBlockStackFrame(curFrame, shared_from_this(), bb));
   }

void StackFrame::assignInitialize(ValueP var, SgExpression *rhs)
   {
     var->assign(evalExpr(rhs), rhs->get_type(), rhs->get_type());
   }

void StackFrame::ctorInitialize(ValueP var, SgConstructorInitializer *ctorInit)
   {
     vector<ValueP> argVals = evalExprListExp(ctorInit->get_args());
     StackFrameP down = newStackFrame(isSgFunctionSymbol(ctorInit->get_declaration()->get_symbol_from_symbol_table()), var);
     down->interpFunction(argVals);
   }

void StackFrame::aggregateInitialize(ValueP var, SgAggregateInitializer *aggInit)
   {
     SgType *initType = aggInit->get_type()->stripTypedefsAndModifiers();
     SgExpressionPtrList &inits = aggInit->get_initializers()->get_expressions();
     if (SgArrayType *at = isSgArrayType(initType))
        {
          SgType *elemType = at->get_base_type();
          const StructLayoutInfo &info = typeLayout(elemType);
          size_t ofs = 0;
          for (SgExpressionPtrList::const_iterator i = inits.begin(); i != inits.end(); ++i, ofs += info.size)
             {
               SgInitializer *init = isSgInitializer(*i);
               ROSE_ASSERT(init != NULL);
               // TODO: handle SgDesignatedInitializers
               ValueP ofsVal (new OffsetValue(var->dereference(), ofs));
               initialize(ofsVal, init);
             }
        }
     else if (SgClassType *ct = isSgClassType(initType))
        {
          const StructLayoutInfo &info = typeLayout(ct);
               vector<StructLayoutEntry>::const_iterator si = info.fields.begin();
          for (SgExpressionPtrList::const_iterator ei = inits.begin(); si != info.fields.end() && ei != inits.end(); ++si, ++ei)
             {
               SgInitializer *init = isSgInitializer(*ei);
               ROSE_ASSERT(init != NULL);
               // TODO: handle SgDesignatedInitializers
               ValueP ofsVal (new OffsetValue(var, si->byteOffset));
               initialize(ofsVal, init);
             }
        }
   }

void StackFrame::initialize(ValueP var, SgInitializer *init)
   {
     if (init)
        {
          switch (init->variantT())
             {
               case V_SgAssignInitializer: assignInitialize(var, isSgAssignInitializer(init)->get_operand()); break;
               case V_SgConstructorInitializer: ctorInitialize(var, isSgConstructorInitializer(init)); break;
               case V_SgAggregateInitializer: aggregateInitialize(var, isSgAggregateInitializer(init)); break;
               default: cerr << "StackFrame::initialize: unrecognized initializer " << init->class_name() << endl; break;
             }
        }
     else
        {
          // TODO: call the default initializer
        }
   }

ValueP StackFrame::evalInitializedName(SgInitializedName *var, blockScopeVars_t &blockScope)
   {
     SgVariableSymbol *sym = isSgVariableSymbol(var->get_symbol_from_symbol_table());
     ROSE_ASSERT(sym != NULL);
     SgInitializer *init = var->get_initializer();
     ValueP binding = newValue(var->get_type(), PStack);
     initialize(binding, init);
     localVarBindings[sym] = binding;
     blockScope.push_back(sym);
     return binding;
   }

ValueP StackFrame::evalVariableDecl(SgVariableDeclaration *vdec, blockScopeVars_t &blockScope)
   {
     SgInitializedNamePtrList &vars = vdec->get_variables();
     ValueP rv;
     for (SgInitializedNamePtrList::iterator varI = vars.begin(); varI != vars.end(); ++varI)
        {
          rv = evalInitializedName(*varI, blockScope);
        }
     return rv;
   }

void StackFrame::evalIfStmt(SgIfStmt *ifStmt, BlockStackFrameP &curFrame)
   {
     curFrame = BlockStackFrameP(new BlockStackFrame(curFrame, shared_from_this(), ifStmt));
     ValueP cond = evalStmtAsBool(ifStmt->get_conditional(), curFrame->scopeVars);
     if (cond->getConcreteValueInt())
        {
          evalStmt(ifStmt->get_true_body(), curFrame);
        }
     else if (ifStmt->get_false_body())
        {
          evalStmt(ifStmt->get_false_body(), curFrame);
        }
   }

struct StackFrame::LoopBlockStackFrame : BlockStackFrame
     {
       LoopBlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgScopeStatement *scope, SgExpression *increment, SgStatement *condition, SgStatement *body) :
               BlockStackFrame(up, sf, scope),
               increment(increment),
               condition(condition),
               body(body),
               first(true) {}

       SgExpression *increment;
       SgStatement *condition, *body;
       blockScopeVars_t loopScopeVars;

       bool first;

       SgStatement *next()
          {
            if (first)
               {
                 first = false;
               }
            else
               {
                 if (increment) sf->evalExpr(increment);
                 sf->destroyScope(loopScopeVars);
               }
            ValueP cond = sf->evalStmtAsBool(condition, loopScopeVars);
            BasePrimTypeValue *condPrim = dynamic_cast<BasePrimTypeValue *>(cond.get());
            return condPrim->getConcreteValueInt() ? body : NULL;
          }

       BlockStackFrameP doContinue()
          {
            return shared_from_this();
          }

       BlockStackFrameP doBreak()
          {
            destroyScope();
            return up;
          }

       void destroyScope()
          {
            sf->destroyScope(loopScopeVars);
            sf->destroyScope(scopeVars);
          }

     };

void StackFrame::evalWhileStmt(SgWhileStmt *whileStmt, BlockStackFrameP &curFrame)
   {
     curFrame = BlockStackFrameP(new LoopBlockStackFrame(curFrame, shared_from_this(), whileStmt, NULL, whileStmt->get_condition(), whileStmt->get_body()));
   }

void StackFrame::evalDoWhileStmt(SgDoWhileStmt *doWhileStmt, BlockStackFrameP &curFrame)
   {
     curFrame = BlockStackFrameP(new LoopBlockStackFrame(curFrame, shared_from_this(), doWhileStmt, NULL, doWhileStmt->get_condition(), doWhileStmt->get_body()));
     evalStmt(doWhileStmt->get_body(), curFrame);
   }

void StackFrame::evalForInitStatement(SgForInitStatement *forInitStmt, blockScopeVars_t &blockScope)
   {
     SgStatementPtrList &initStmts = forInitStmt->get_init_stmt();
     for (SgStatementPtrList::iterator stmtI = initStmts.begin(); stmtI != initStmts.end(); ++stmtI)
        {
          evalStmtAsExpr(*stmtI, blockScope);
        }
   }

void StackFrame::evalForStatement(SgForStatement *forStmt, BlockStackFrameP &curFrame)
   {
     curFrame = BlockStackFrameP(new LoopBlockStackFrame(curFrame, shared_from_this(), forStmt, forStmt->get_increment(), forStmt->get_test(), forStmt->get_loop_body()));
     evalForInitStatement(forStmt->get_for_init_stmt(), curFrame->scopeVars);
   }

void StackFrame::evalReturnStmt(SgReturnStmt *retStmt, BlockStackFrameP &curFrame)
   {
     SgType *returnType = funSym->get_declaration()->get_type()->get_return_type();
     returnValue = evalExpr(retStmt->get_expression())->copy(returnType, PTemp, shared_from_this());
     do {
          curFrame->destroyScope();
          curFrame = curFrame->up;
     } while (curFrame.get());
   }

void StackFrame::evalStmt(SgStatement *stmt, BlockStackFrameP &curFrame)
   {
     blockScopeVars_t &blockScope = curFrame->scopeVars;
     try
        {
          switch (stmt->variantT())
             {
               case V_SgBasicBlock: evalBasicBlock(isSgBasicBlock(stmt), curFrame); break;
               case V_SgVariableDeclaration: evalVariableDecl(isSgVariableDeclaration(stmt), blockScope); break;
               case V_SgExprStatement: evalExpr(isSgExprStatement(stmt)->get_expression()); break;
               case V_SgIfStmt: evalIfStmt(isSgIfStmt(stmt), curFrame); break;
               case V_SgWhileStmt: evalWhileStmt(isSgWhileStmt(stmt), curFrame); break;
               case V_SgDoWhileStmt: evalDoWhileStmt(isSgDoWhileStmt(stmt), curFrame); break;
               case V_SgForStatement: evalForStatement(isSgForStatement(stmt), curFrame); break;
               case V_SgBreakStmt: curFrame = curFrame->doBreak(); break;
               case V_SgContinueStmt: curFrame = curFrame->doContinue(); break;
               case V_SgReturnStmt: evalReturnStmt(isSgReturnStmt(stmt), curFrame); break;
               default: cerr << "StackFrame::evalStmt: unhandled statement " << stmt->sage_class_name() << " encountered, skipping" << endl;
             }
        }
     catch (InterpError &ie)
        {
          SgStatement *&frame = ie.callStack[ie.callStack.size()-1];
          if (frame == NULL)
             {
               frame = stmt;
             }
          throw;
        }
     if (interp()->trace)
        {
          cout << "after evalStmt(" << stmt->unparseToString() << "): ";
          if (thisBinding.get())
             {
               cout << "thisBinding = " << thisBinding->show() << ", ";
             }
          cout << "localVarBindings = ";
          dumpBindings(std::cout, localVarBindings);
          cout << endl;
        }
   }

ValueP StackFrame::evalStmtAsExpr(SgStatement *stmt, blockScopeVars_t &blockScope)
   {
     switch (stmt->variantT())
        {
          case V_SgVariableDeclaration: return evalVariableDecl(isSgVariableDeclaration(stmt), blockScope);
          case V_SgExprStatement: return evalExpr(isSgExprStatement(stmt)->get_expression());
          default:
             throw InterpError("StackFrame::evalStmtAsExpr: unexpected statement " + stmt->class_name() + " encountered");
        }
   }

ValueP StackFrame::evalStmtAsBool(SgStatement *stmt, blockScopeVars_t &blockScope)
   {
     return evalStmtAsExpr(stmt, blockScope);
   }

void StackFrame::mainEvalLoop(BlockStackFrameP &curFrame)
   {
     while (curFrame.get())
        {
          SgStatement *next;
          do {
               next = curFrame->next();
               if (!next)
                  {
                    curFrame->destroyScope();
                    curFrame = curFrame->up;
                  }
          } while (!next && curFrame.get());
          if (next) evalStmt(next, curFrame);
        }
   }

ValueP StackFrame::interpFunction(const vector<ValueP> &actualParams)
   {
     SgFunctionDeclaration *decl = funSym->get_declaration();
     SgFunctionDeclaration *defDecl = isSgFunctionDeclaration(decl->get_definingDeclaration());
     ROSE_ASSERT(defDecl != NULL);
     language = SageInterface::getEnclosingFileNode(defDecl)->get_outputLanguage();
     SgFunctionDefinition *def = defDecl->get_definition();
     ROSE_ASSERT(def != NULL);
     BlockStackFrameP fnBlock (new BlockStackFrame(BlockStackFrameP(), shared_from_this(), def));
     SgInitializedNamePtrList &formalParams = defDecl->get_args();
     ROSE_ASSERT(formalParams.size() == actualParams.size());
          SgInitializedNamePtrList::iterator fpI = formalParams.begin();
     for (vector<ValueP>::const_iterator apI = actualParams.begin();
                     apI != actualParams.end();
                     ++fpI, ++apI)
        {
          SgVariableSymbol *fpSym = isSgVariableSymbol((*fpI)->get_symbol_from_symbol_table());
          if (fpSym != NULL) // fpSym is NULL if the parameter variable is anonymous
             {
               localVarBindings[fpSym] = (*apI)->copy((*fpI)->get_type(), PStack, shared_from_this(), true);
               fnBlock->scopeVars.push_back(fpSym);
             }
        }
  // if this is a constructor, initialize fields according to the ctors
  // TODO: initialize members in the correct order according to C++ rules
     if (defDecl->get_specialFunctionModifier().isConstructor())
        {
          SgMemberFunctionDeclaration *memDefDecl = isSgMemberFunctionDeclaration(defDecl);
          ROSE_ASSERT(memDefDecl != NULL);
          SgInitializedNamePtrList &ctors = memDefDecl->get_ctors();
          for (SgInitializedNamePtrList::const_iterator ctorI = ctors.begin(); ctorI != ctors.end(); ++ctorI)
             {
               SgInitializedName *ctor = *ctorI;
               switch (ctor->get_preinitialization())
                  {
                    case SgInitializedName::e_virtual_base_class:
                    case SgInitializedName::e_nonvirtual_base_class:
                       initialize(thisBinding, ctor->get_initializer());
                       break;
                    case SgInitializedName::e_data_member:
                          {
#if 0
                            SgSymbol *memSym = ctor->get_symbol_from_symbol_table();
                            ROSE_ASSERT(memSym != NULL);
                            SgVariableSymbol *memVarSym = isSgVariableSymbol(memSym);
                            ROSE_ASSERT(memVarSym != NULL);
#endif
                         // get_symbol_from_symbol_table does not work for SgInitializedNames which are preinitializers
                            SgVariableSymbol *memVarSym = ctor->get_scope()->lookup_variable_symbol(ctor->get_name());
                            ROSE_ASSERT(memVarSym != NULL);
                            size_t offset = sageOffsetOf(thisBinding, memDefDecl->get_associatedClassDeclaration()->get_type(), memVarSym);
                            initialize(ValueP(new OffsetValue(thisBinding, offset)), ctor->get_initializer());
                            break;
                          }
                    default:
                       cerr << "Unrecognised preinitialization " << ctor->get_preinitialization() << endl;
                       break;
                  }
             }
        }
     evalBasicBlock(def->get_body(), fnBlock);
     mainEvalLoop(fnBlock);
     return returnValue;
   }

StackFrame::~StackFrame() {}

void Interpretation::prePrimAssign(ValueP lhs, const_ValueP rhs) {}

void Interpretation::parseCommandLine(vector<string> &args)
   {
     trace = CommandlineProcessing::isOption(args, "-interp:", "trace", true);
   }

Interpretation::~Interpretation() {}

};
