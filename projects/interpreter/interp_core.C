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

     if( base->get_definingDeclaration() == clDecl->get_definingDeclaration() )
          return true;

     SgClassDefinition* clDef = isSgClassDeclaration(clDecl->get_definingDeclaration())->get_definition();

     if(clDef == NULL)
          return false;

     SgBaseClassPtrList&  baseClLst = clDef->get_inheritances();

     for(SgBaseClassPtrList::iterator i = baseClLst.begin();
                     i != baseClLst.end(); ++i )
        {

          SgClassDeclaration* baseCl = (*i)->get_base_class();

          if( isBaseClassOf( base, baseCl ) == true  )
               return true;
        } 

     return false;
   }
 
static SgScopeStatement *symbolScope(SgSymbol *symbol)
   {
     switch (symbol->variantT())
        {
          case V_SgClassSymbol: return static_cast<SgClassSymbol *>(symbol)->get_declaration()->get_scope(); break;
          case V_SgVariableSymbol: return static_cast<SgVariableSymbol *>(symbol)->get_declaration()->get_scope(); break;
          default: throw Interp::InterpError("symbolScope: unexpected symbol type " + symbol->class_name());
        }
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

SgVariableSymbol *firstNamedMember(SgClassType *ct)
   {
     SgClassDefinition *cdef = isSgClassDeclaration(ct->get_declaration()->get_definingDeclaration())->get_definition();
     SgDeclarationStatementPtrList &members = cdef->get_members();
     for (SgDeclarationStatementPtrList::iterator i = members.begin(); i != members.end(); ++i)
        {
          if (SgVariableDeclaration *vd = isSgVariableDeclaration(*i))
             {
               SgInitializedName *in = vd->get_variables().front();
               if (in->get_name() == "") continue;
               SgVariableSymbol *vs = isSgVariableSymbol(in->search_for_symbol_from_symbol_table());
               ROSE_ASSERT(vs != NULL);
               return vs;
             }
        }
     return NULL;
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
       callStack.push_back(Frame(StackFrameP(), NULL));
     }

void InterpError::dumpCallStack(ostream &out)
   {
     Interpretation *interp = NULL;
     bool trace = !callStack.empty()
                && callStack.back().first
      && (interp = callStack.back().first->interp())->errorTrace;
     for (size_t i = 0; i < callStack.size(); i++)
        {
          Frame &frame = callStack[i];
          SgStatement *stmt = frame.second;
          out << " at ";
          SgNode *parent = stmt;
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
          Sg_File_Info *soc = stmt->get_startOfConstruct();
          if (soc)
             {
               out << " (" << soc->get_filenameString() << ":" << soc->get_line() << ")";
             }
          if (trace && frame.first)
             {
               out << ", localVarBindings = ";
               dumpBindings(out, frame.first->localVarBindings);
             }
          out << endl;
        }
     if (trace)
        {
          out << "globalVarBindings = ";
          dumpBindings(out, interp->globalVarBindings);
          out << endl;
        }
   }

FieldVisitor::~FieldVisitor() {}

Value::Value(Position pos, StackFrameP owner) : pos(pos), owner(owner)
   {
#if 0
     if (owner && owner->interp()->trace)
          cout << "Value::Value @" << this << endl;
#endif
   }

Value::~Value()
   {
#if 0
     if (owner && owner->interp()->trace)
          cout << "Value::~Value @" << this << endl;
#endif
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
          if (!field)
             {
               throw InterpError("fieldAtOffset returned NULL!");
             }
        }
     while (fieldp != field);
     return field;
   };

const_ValueP Value::primAtOffset(size_t offset) const
   {
     return const_cast<Value *>(this)->primAtOffset(offset);
   };

void Value::forEachSubfield(FieldVisitor &visit, long offset) {}

void Value::forEachPrim(FieldVisitor &visit, long offset)
   {
     visit(offset, shared_from_this());
   }

bool Value::hasDynamicTypeAtOffset(SgClassType *t, size_t offset) const
   {
     return false;
   }

size_t Value::backwardValidity() const { return 0; }

ValueP Value::dereference() const
   {
     ValueP val = unsafeDereference();
     if (val == ValueP())
          throw InterpError("Attempt to dereference a NULL pointer!");
     return val;
   }

ValueP Value::unsafeDereference() const
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

bool BaseCompoundValue::getConcreteValueBool() const
   {
     return prim()->getConcreteValueBool();
   }

char BaseCompoundValue::getConcreteValueChar() const
   {
     return prim()->getConcreteValueChar();
   }

double BaseCompoundValue::getConcreteValueDouble() const
   {
     return prim()->getConcreteValueDouble();
   }

float BaseCompoundValue::getConcreteValueFloat() const
   {
     return prim()->getConcreteValueFloat();
   }

int BaseCompoundValue::getConcreteValueInt() const
   {
     return prim()->getConcreteValueInt();
   }

long double BaseCompoundValue::getConcreteValueLongDouble() const
   {
     return prim()->getConcreteValueLongDouble();
   }

long int BaseCompoundValue::getConcreteValueLong() const
   {
     return prim()->getConcreteValueLong();
   }

long long int BaseCompoundValue::getConcreteValueLongLong() const
   {
     return prim()->getConcreteValueLongLong();
   }

short BaseCompoundValue::getConcreteValueShort() const
   {
     return prim()->getConcreteValueShort();
   }

unsigned char BaseCompoundValue::getConcreteValueUnsignedChar() const
   {
     return prim()->getConcreteValueUnsignedChar();
   }

unsigned int BaseCompoundValue::getConcreteValueUnsignedInt() const
   {
     return prim()->getConcreteValueUnsignedInt();
   }

unsigned long long int BaseCompoundValue::getConcreteValueUnsignedLongLong() const
   {
     return prim()->getConcreteValueUnsignedLongLong();
   }

unsigned long BaseCompoundValue::getConcreteValueUnsignedLong() const
   {
     return prim()->getConcreteValueUnsignedLong();
   }

unsigned short BaseCompoundValue::getConcreteValueUnsignedShort() const
   {
     return prim()->getConcreteValueUnsignedShort();
   }

SgType *Value::defaultType() const
   {
     throw InterpError("The defaultType method is not implemented for this Value");
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
    return assign(eval##opname(rhs, lhsApt, rhsApt), lhsApt, lhsApt); \
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
GDEFINE_UNDEF_UNOP_FN(--,PrefixMinusMinusOp,)

#undef GDEFINE_UNDEF_UNOP_FN
#undef DEFINE_UNDEF_UNOP_FN
#undef GDEFINE_UNDEF_BINOP_FN
#undef DEFINE_UNDEF_BINOP_FN
#undef DEFINE_UNDEF_BINOP_FNS

ValueP Value::evalPostfixPlusPlusOp(SgType *apt)
   {
     ValueP result = copy(apt, PTemp, owner);
     evalPrefixPlusPlusOp(apt);
     return result;
   }

ValueP Value::evalPostfixMinusMinusOp(SgType *apt)
   {
     ValueP result = copy(apt, PTemp, owner);
     evalPrefixMinusMinusOp(apt);
     return result;
   }

void Value::setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant)
   {
     throw InterpError("Cannot set discriminant of this value");
   }

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

void BaseCompoundValue::setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant)
   {
     fieldAtOffset(0)->setDiscriminantAtOffset(offset, discriminant);
   }

#define DEFINE_PV_BOOL_BINOP_FN(op,opname) \
ValueP PointerValue::eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const \
   { \
     const PointerValue *rhsPtr = dynamic_cast<const PointerValue *>(rhs->prim().get()); \
     lhsApt = lhsApt->stripTypedefsAndModifiers(); \
     rhsApt = rhsApt->stripTypedefsAndModifiers(); \
     if ((!isSgPointerType(lhsApt) && !isSgArrayType(lhsApt)) \
      || (!isSgPointerType(rhsApt) && !isSgArrayType(rhsApt))) \
        { \
          throw InterpError("Operator " #op " applied to non-pointer type"); \
        } \
     if (!rhsPtr) \
        { \
          throw InterpError("Operator " #op " between incompatible types"); \
        } \
     if (this->valid() && rhsPtr->valid()) \
          return ValueP(mkBoolean(this->target.get() op rhsPtr->target.get(), PTemp, this->owner)); \
     else \
          return ValueP(mkBoolean(PTemp, this->owner)); \
   }

FOREACH_BOOL_BINARY_PRIMOP(DEFINE_PV_BOOL_BINOP_FN)

#undef DEFINE_PV_BOOL_BINOP_FN

ValueP PointerValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     rhsApt = rhsApt->stripTypedefsAndModifiers();
     if (isSgFunctionType(rhsApt))
        {
       // implicit function-to-pointer conversion
          isValid = true;
          target = boost::const_pointer_cast<Value, const Value>(rhs);
          return shared_from_this();
        }
     lhsApt = lhsApt->stripTypedefsAndModifiers();
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
     if (rhsPtr->isValid)
        {
          isValid = true;
          target = rhsPtr->target;
        }
     else
          isValid = false;
     return shared_from_this();
   }

string PointerValue::show() const
   {
     if (!isValid)
          return "<<undefined>>";
     if (target.get() == NULL)
          return "<<null-ptr>>";
     stringstream ss;
     // TODO: This won't work for circular structures - will need to think of something else
     ss << target << " &{" << target->show() << "}";
     return ss.str();
   }

size_t PointerValue::forwardValidity() const { return sizeof(void*); }

ValueP PointerValue::unsafeDereference() const
   {
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
     ValueP ofs = adjustedOffset(lhsApt, rhs->getConcreteValueInt());
     return ValueP(new PointerValue(ofs, PTemp, owner));
   }

ValueP PointerValue::evalSubtractOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const
   {
     if (SgPointerType *rhsPtrT = isSgPointerType(rhsApt->stripTypedefsAndModifiers()))
        {
          /* The rules for pointer subtraction are C99 6.5.6p9, in particular:
              "When two pointers are subtracted, both shall point to elements of the same array object,
              or one past the last element of the array object..." */
          if (rhs->unsafeDereference() == ValueP())
             {
            /* Subtracting the NULL pointer from a pointer is undefined (see above) but some programs use a result to determine alignment */
               cerr << "Warning: pointer difference from NULL requested" << endl;
               return ValueP(new IntegralPrimTypeValue<ptrdiff_t>(0, PTemp, owner));
             }
          SgType *rhsBaseT = rhsPtrT->get_base_type();
          const StructLayoutInfo &info = typeLayout(rhsBaseT);
          const_ValueP lhsTarget, rhsTarget;
          size_t lhsOffset, rhsOffset;
          target->targetOffset(lhsTarget, lhsOffset);
          rhs->dereference()->targetOffset(rhsTarget, rhsOffset);
          if (lhsTarget != rhsTarget)
               throw InterpError("Pointer difference requested, but pointers point to different areas");
          ptrdiff_t diff = (lhsOffset-rhsOffset)/info.size;
          return ValueP(new IntegralPrimTypeValue<ptrdiff_t>(diff, PTemp, owner));
        }
     else
        {
          ValueP ofs = adjustedOffset(lhsApt, -rhs->getConcreteValueInt());
          return ValueP(new PointerValue(ofs, PTemp, owner));
        }
   }

ValueP PointerValue::evalPrefixPlusPlusOp(SgType *apt)
   {
     target = adjustedOffset(apt, 1);
     return shared_from_this();
   }

ValueP PointerValue::evalPrefixMinusMinusOp(SgType *apt)
   {
     target = adjustedOffset(apt, -1);
     return shared_from_this();
   }

SgType *PointerValue::defaultType() const
   {
     return SgPointerType::createType(SgTypeVoid::createType());
   }

string SymbolValue::show() const
   {
     return getSymbol()->unparseToString();
   }

ValueP SymbolValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     throw InterpError("Attempt to assign to a SymbolValue");
   }

bool SymbolValue::valid() const
   {
     return true;
   }

void SymbolValue::destroy()
   {
     throw InterpError("Attempt to destroy a SymbolValue");
   }

ValueP Value::evalDotExp(ValueP lhs, SgType *lhsApt) const
   {
     throw InterpError("Operator . not defined for this type of value");
   }

ValueP Value::evalVarArgOp(SgType *apt)
   {
     throw InterpError("Vararg operator not defined for this type of value");
   }

void Value::evalVarArgEndOp()
   {
     throw InterpError("Vararg end operator not defined for this type of value");
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

ValueP BaseCompoundValue::unsafeDereference() const
   {
     return prim()->unsafeDereference();
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

bool BaseCompoundValue::valid() const
   {
     return prim()->valid();
   }

ValueP BaseCompoundValue::evalVarArgOp(SgType *apt)
   {
     return prim()->evalVarArgOp(apt);
   }

void BaseCompoundValue::evalVarArgEndOp()
   {
     prim()->evalVarArgEndOp();
   }

OffsetValue::OffsetValue(ValueP base, size_t offset) : BaseCompoundValue(base->pos, base->owner)
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

void OffsetValue::destroy()
   {
     throw InterpError("OffsetValues cannot be destroyed");
   }

void OffsetValue::targetOffset(const_ValueP &target, size_t &offset) const
   {
     base->targetOffset(target, offset);
     offset += this->offset;
   }

void UnionDiscriminantValue::destroy()
   {
     throw InterpError("UnionDiscriminantValues cannot be destroyed");
   }

void UnionDiscriminantValue::targetOffset(const_ValueP &target, size_t &offset) const
   {
     base->targetOffset(target, offset);
   }

std::string CompoundValue::show() const
   {
     stringstream ss;
     if (dynamicType != NULL)
        {
          ss << dynamicType->get_name().getString() << " ";
        }
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
     else if (SgArrayType *at = isSgArrayType(lhsApt))
        {
          SgType *baseType = at->get_base_type();
          const StructLayoutInfo &info = typeLayout(baseType);
          size_t arrSize = forwardValidity();
          for (size_t ofs = 0; ofs < arrSize; ofs += info.size)
             {
               ValueP lhsOfs(new OffsetValue(shared_from_this(), ofs));
               ValueP rhsOfs(new OffsetValue(const_pointer_cast<Value, const Value>(rhs), ofs));
               lhsOfs->assign(rhsOfs, baseType, baseType);
             }
          return shared_from_this();
        }
     else
        {
          owner->interp()->prePrimAssign(shared_from_this(), rhs, lhsApt, rhsApt);
          return primAssign(rhs, lhsApt, rhsApt);
        }
   }

void CompoundValue::forEachSubfield(FieldVisitor &visit, long offset)
   {
     for (fieldMap_t::const_iterator fieldI = fields.begin(); fieldI != fields.end(); ++fieldI)
        {
          visit(offset+fieldI->first, fieldI->second);
        }
   }

void OffsetValue::forEachSubfield(FieldVisitor &visit, long offset)
   {
     base->forEachSubfield(visit, offset - this->offset);
   }

ValueP Value::copy(SgType *apt, Position pos, StackFrameP owner, Context ctx) const
   {
     ValueP newVal = owner->newValue(apt, pos, ctx);
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
          return primCopy(apt, pos, owner, ctx);
        }
   }

ValueP Value::primCopy(SgType *apt, Position pos, StackFrameP owner, Context ctx) const
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

void Value::targetOffset(const_ValueP &target, size_t &offset) const
   {
     target = shared_from_this();
     offset = 0;
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

CompoundValue::CompoundValue(SgClassType *dynamicType, Position pos, StackFrameP owner) : BaseCompoundValue(pos, owner), dynamicType(dynamicType)
     {
       const StructLayoutInfo &info = typeLayout(dynamicType);
       size = info.size;
       for (std::vector<StructLayoutEntry>::const_iterator fieldI = info.fields.begin(); fieldI != info.fields.end(); ++fieldI)
          {
            if (fieldI->decl)
               {
                 SgType *fldType = fieldType(fieldI->decl);
                 fields[fieldI->byteOffset] = owner->newValue(fldType, pos, CField);
               }
          }
     }

CompoundValue::CompoundValue(SgClassType *dynamicType, fieldMap_t fields, Position pos, StackFrameP owner, size_t size) : 
        BaseCompoundValue(pos, owner), 
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

struct VisitPrimSubfieldsVisitor : FieldVisitor
     {
       FieldVisitor &visit;

       VisitPrimSubfieldsVisitor(FieldVisitor &visit) : visit(visit) {}

       void operator()(long offset, ValueP val)
          {
            val->forEachPrim(visit, offset);
          }
     };

void BaseCompoundValue::forEachPrim(FieldVisitor &visit, long offset)
   {
     VisitPrimSubfieldsVisitor vpsv(visit);
     forEachSubfield(vpsv, offset);
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
     if (offset == 0 && dynamicType != NULL) types.push_back(dynamicType);
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

void CompoundValue::setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant)
   {
     fieldAtOffset(offset)->setDiscriminant(discriminant);
   }

string UnionValue::show() const
   {
     if (!currDiscriminant || !storage)
        {
          return "<<undefined union, type=" + unionType->unparseToString() + ">>";
        }
     return "{" + currDiscriminant->get_name().getString() + " <- " + storage->show() + "}";
   }

size_t UnionValue::forwardValidity() const
   {
     return storage ? storage->forwardValidity() : 0;
   }

void UnionValue::forEachSubfield(FieldVisitor &visit, long offset)
   {
     if (storage)
          storage->forEachSubfield(visit, offset);
   }

ValueP UnionValue::fieldAtOffset(size_t offset)
   {
     if (!storage)
          throw InterpError("Attempt to access uninitialised storage");
     if (offset == 0)
          return storage;
     return storage->fieldAtOffset(offset);
   }

const_ValueP UnionValue::fieldAtOffset(size_t offset) const
   {
     return const_cast<UnionValue *>(this)->fieldAtOffset(offset);
   }

void UnionValue::setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant)
   {
     if (offset == 0)
        {
          if (discriminant == currDiscriminant) return;
          SgClassDefinition *clsDef = isSgClassDefinition(symbolScope(discriminant));
          SgClassDeclaration *clsDecl = clsDef->get_declaration();
       // if (clsType == unionType)
          if (clsDecl == unionType->get_declaration()->get_definingDeclaration())
             {
               storage = owner->newValue(discriminant->get_type(), pos, CField);
               currDiscriminant = discriminant;
               return;
             }
        }
     storage->setDiscriminantAtOffset(offset, discriminant);
   }

void UnionValue::destroy()
   {
     if (storage)
          storage->destroy();
   }

DynamicValue::DynamicValue(size_t size, Position pos, StackFrameP owner) :
        BaseCompoundValue(pos, owner),
        size(size),
        storageIsCanary(true),
        storage(owner->newValue(SgTypeInt::createType(), pos)) {}

string DynamicValue::show() const
   {
     if (storageIsCanary)
        {
          stringstream ss;
          ss << "<<undefined dynamic, size=" << size << ", canary valid=" << storage->valid() << ">>";
          return ss.str();
        }
     return "{<<dynamic>> " + storage->show() + "}";
   }

size_t DynamicValue::forwardValidity() const
   {
     return size;
   }

void DynamicValue::forEachSubfield(FieldVisitor &visit, long offset)
   {
     if (storage)
        {
          if (storageIsCanary)
               visit(offset, storage);
          else
               storage->forEachSubfield(visit, offset);
        }
   }

ValueP DynamicValue::fieldAtOffset(size_t offset)
   {
     if (!storage)
          throw InterpError("Attempt to access uninitialised storage");
     if (offset == 0)
          return storage;
     return storage->fieldAtOffset(offset);
   }

const_ValueP DynamicValue::fieldAtOffset(size_t offset) const
   {
     return const_cast<DynamicValue *>(this)->fieldAtOffset(offset);
   }

void DynamicValue::reinterpret(SgType *newType)
   {
     if (storageType == newType || newType->variantT() == V_SgTypeVoid)
          return;

     if (!storageIsCanary)
        {
          cerr << "Warning: Attempt to re-reinterpret a DynamicValue to " << newType->unparseToString() << endl;
          return;
        }

     const StructLayoutInfo &info = typeLayout(newType);
     size_t elems = size/info.size;
     if (elems*info.size != size)
        {
          cerr << "Warning: DynamicValue::reinterpret: size of storage (" << size << ") not an exact multiple of size of dynamic type " << newType->unparseToString() << " (" << info.size << ")" << endl;
        }

     ValueP newStorage (new CompoundValue(newType, elems, pos, owner));
     if (storage->valid())
        {
          interpMemset(owner, newStorage, storage, SgTypeInt::createType(), size);
        }
     storage = newStorage;
     storageIsCanary = false;
     storageType = newType;
   }

void DynamicValue::realloc(size_t newSize)
   {
     size = newSize;
     if (storageIsCanary)
          return;

     CompoundValue *cv = dynamic_cast<CompoundValue *>(storage.get());
     const StructLayoutInfo &info = typeLayout(storageType);
     size_t oldElems = cv->size/info.size, newElems = newSize/info.size;
     if (newElems > oldElems)
        {
          for (size_t elem = oldElems; elem < newElems; ++elem)
             {
               cv->fields[elem*info.size] = owner->newValue(storageType, PTemp);
             }
        }
     else if (newElems < oldElems)
        {
          for (size_t elem = newElems; elem < oldElems; ++elem)
             {
               cv->fields[elem*info.size]->destroy();
               cv->fields.erase(elem*info.size);
             }
        }
     cv->size = newElems * info.size;
   }

void DynamicValue::destroy()
   {
     if (storage)
          storage->destroy();
   }

bool BasePrimValue::valid() const
   {
     return isValid;
   }

void BasePrimValue::destroy()
   {
     isValid = false;
   }

string VAListValue::show() const
   {
     if (isValid)
        {
          stringstream ss;
          ss << "<<va_list ";
          for (VAList::const_iterator i = vaList->begin(); i != vaList->end(); ++i)
             {
               if (i == vaListPos)
                    ss << "-> ";
               ss << (*i)->show() << " ";
               if (i == vaListPos)
                    ss << "<- ";
             }
          ss << ">>";
          return ss.str();
        }
     else
          return "<<undefined va_list>>";
   }

ValueP VAListValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     const VAListValue *rhsVAList = dynamic_cast<const VAListValue *>(rhs.get());
     if (rhsVAList == NULL)
        {
          throw InterpError("VAListValue::primAssign: rhs not a VAListValue");
        }
     isValid = rhs->valid();
     if (isValid)
        {
          vaList = rhsVAList->vaList;
          vaListPos = rhsVAList->vaListPos;
        }
     return shared_from_this();
   }

size_t VAListValue::forwardValidity() const { return 1; }

ValueP VAListValue::evalVarArgOp(SgType *apt)
   {
     if (!isValid)
        {
          throw InterpError("Called va_arg on undefined va_list");
        }
     if (vaListPos == vaList->end())
        {
          throw InterpError("Called va_arg past the end of the argument list");
        }
     return *vaListPos++;
   }

void VAListValue::evalVarArgEndOp()
   {
     isValid = false;
     vaList = VAListP();
   }

void dumpBindings(std::ostream &out, const varBindings_t &bindings)
   {
     out << "{";
     for (varBindings_t::const_iterator bindI = bindings.begin(); bindI != bindings.end(); ++bindI)
        {
          if (bindI != bindings.begin()) out << ", ";
          out << bindI->first << " " << bindI->first->get_name().getString() << " => " << bindI->second->show();
        }
     out << "}";
   }

void StackFrame::dump()
   {
     dumpBindings(cout, localVarBindings);
     cout << endl;
   }

SgStatement *StackFrame::BlockStackFrame::next() { return NULL; }

void StackFrame::BlockStackFrame::destroyScope()
   {
     sf->destroyScope(scopeVars);
   }

StackFrame::BlockStackFrameP StackFrame::BlockStackFrame::doContinue()
   {
     destroyScope();
     ROSE_ASSERT(up.get() != NULL);
     return up->doContinue();
   }

StackFrame::BlockStackFrameP StackFrame::BlockStackFrame::doBreak()
   {
     destroyScope();
     ROSE_ASSERT(up.get() != NULL);
     return up->doBreak();
   }

StackFrame::BlockStackFrame::~BlockStackFrame() {}

CompoundValue::CompoundValue(SgType *baseType, size_t index, Position pos, StackFrameP owner) : BaseCompoundValue(pos, owner), dynamicType(NULL)
   {
     const StructLayoutInfo &info = typeLayout(baseType);
     size = index * info.size;
     for (size_t i = 0; i < index; ++i)
        {
          fields[i * info.size] = owner->newValue(baseType, pos);
        }
   }

ValueP StackFrame::newArray(SgArrayType *at, Position pos, Context ctx)
   {
     SgExpression *index = at->get_index();
     if (pos == PTemp   /* temporaries are only pointers */
      || index == NULL) /* this is an incomplete type, so it should be represented as a pointer */
          return ValueP(new PointerValue(pos, shared_from_this()));
     ValueP indexVal = evalExpr(index);
     return ValueP(new CompoundValue(at->get_base_type(), indexVal->getConcreteValueInt(), pos, shared_from_this()));
   }

ValueP StackFrame::newClassValue(SgClassType *ct, Position pos)
   {
     SgClassDeclaration *cd = isSgClassDeclaration(ct->get_declaration());
     if (cd->get_class_type() == SgClassDeclaration::e_union)
        {
          return ValueP(new UnionValue(ct, pos, shared_from_this()));
        }
     else
        {
          return ValueP(new CompoundValue(ct, pos, shared_from_this()));
        }
   }

ValueP StackFrame::newTypedefValue(SgTypedefType *tt, Position pos, Context ctx)
   {
     SgTypedefDeclaration *td = isSgTypedefDeclaration(tt->get_declaration());
     if (td->get_name() == "va_list" && td->get_scope()->variantT() == V_SgGlobal)
        {
          return ValueP(new VAListValue(pos, shared_from_this()));
        }
     return newValue(td->get_base_type(), pos, ctx);
   }

ValueP StackFrame::newValue(SgType *t, Position pos, Context ctx)
   {
     switch (t->variantT())
        {
          case V_SgTypedefType: return newTypedefValue(static_cast<SgTypedefType *>(t), pos, ctx);
          case V_SgModifierType: return newValue(static_cast<SgModifierType *>(t)->get_base_type(), pos, ctx);
          case V_SgTypeBool: return ValueP(new BoolValue(pos, shared_from_this()));
          case V_SgTypeChar: return ValueP(new CharValue(pos, shared_from_this()));
          case V_SgTypeDouble: return ValueP(new DoubleValue(pos, shared_from_this()));
          case V_SgTypeFloat: return ValueP(new FloatValue(pos, shared_from_this()));
          case V_SgEnumType:
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
          case V_SgClassType: return newClassValue(isSgClassType(t), pos);
          case V_SgArrayType: return newArray(isSgArrayType(t), pos, ctx);
          case V_SgTypeString:
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

ValueP StackFrame::evalVarRefExp(ValueP symTabEntry, SgType *apt)
   {
     return symTabEntry;
   }

ValueP StackFrame::evalVarRefExp(SgVarRefExp *vr)
   {
     SgVariableSymbol *sym = vr->get_symbol();
     SgInitializedName *in = sym->get_declaration();
     SgClassDefinition *clsDef = isSgClassDefinition(in->get_scope());
     if (clsDef && !in->get_storageModifier().isStatic())
        {
       // a non-static member variable
          return ValueP(new NonstaticFieldValue(sym, PTemp, shared_from_this()));
        }
     else
        {
       // static, local or global variable
          varBindings_t::const_iterator vari = localVarBindings.find(sym);
          if (vari != localVarBindings.end()) return evalVarRefExp(vari->second, vr->get_type());
          varBindings_t &globalVarBindings = currentInterp->globalVarBindings;
          vari = globalVarBindings.find(sym);
          if (vari != globalVarBindings.end()) return evalVarRefExp(vari->second, vr->get_type());
          bool isGlobal = isGlobalVar(sym->get_declaration());
          cout << "Oh no, magic happened (sym = " << sym << ", name = " << sym->get_name().getString() << ", isGlobal = " << isGlobal << ")" << endl;
          varBindings_t &thisVarBindings = isGlobal ? globalVarBindings : localVarBindings;
          ValueP newVal = newValue(sym->get_type(), isGlobal ? PGlob : PStack);
          thisVarBindings[sym] = newVal;
          return evalVarRefExp(newVal, vr->get_type());
        }
   }

ValueP StackFrame::evalFunctionRefExp(SgFunctionSymbol *sym)
   {
     const Interpretation::builtins_t &builtins = interp()->builtinFns();
     string qualName = sym->get_declaration()->get_qualified_name().getString();
     if (qualName[0] != ':') /* Bug: implicitly defined functions lack name qualification */
          qualName = "::" + qualName;

     Interpretation::builtins_t::const_iterator bi = builtins.find(qualName);

     if (bi != builtins.end())
        {
          // XXX: hack to give the FunctionValue a valid owner
          bi->second->owner = shared_from_this();
          return bi->second;
        }

  // Note: this should really be a shared PGlob so that pointer comparisons etc will work
     return ValueP(new StaticFunctionValue(sym, PTemp, shared_from_this()));
   }

Interpretation::Interpretation() : _builtinFns(NULL) {}

const Interpretation::builtins_t &Interpretation::builtinFns() const
   {
     if (_builtinFns == NULL)
        {
          _builtinFns = new builtins_t;
          registerBuiltinFns(*_builtinFns);
        }
     return *_builtinFns;
   }

void Interpretation::registerBuiltinFns(builtins_t &builtins) const
   {
     builtins["::memcpy"] = ValueP(new memcpyFnValue(PGlob, StackFrameP()));
     builtins["::memcmp"] = ValueP(new memcmpFnValue(PGlob, StackFrameP()));
     builtins["::memset"] = ValueP(new memsetFnValue(PGlob, StackFrameP()));
     builtins["::malloc"] = ValueP(new mallocFnValue(PGlob, StackFrameP()));
     builtins["::realloc"] = ValueP(new reallocFnValue(PGlob, StackFrameP()));
     builtins["::free"] = ValueP(new freeFnValue(PGlob, StackFrameP()));
     builtins["::__builtin_expect"] = ValueP(new __builtin_expectFnValue(PGlob, StackFrameP()));
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
     return ValueP(new PointerValue(evalExpr(opd, false), PTemp, shared_from_this()));
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

void Value::reinterpret(SgType *newType)
   {
     cout << "NOTE: value reinterpreted to " << newType->unparseToString() << endl;
   }

void Value::realloc(size_t newSize)
   {
     throw InterpError("This type of value does not support reallocation");
   }

ValueP PointerValue::evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType)
   {
     fromType = fromType->stripTypedefsAndModifiers();
     toType = toType->stripTypedefsAndModifiers();
     if (isSgPointerType(toType) && isSgTypeInt(fromType))
        {
          if (fromVal->prim()->getConcreteValueInt() != 0)
             {
               throw InterpError("Attempt to cast a non-zero integer to a pointer");
             }
          setTarget(ValueP());
          return shared_from_this();
        }
     else if (isSgPointerType(toType) && isSgFunctionType(fromType))
        {
       // implicit function-to-pointer conversion
          setTarget(fromVal);
          return shared_from_this();
        }
     else if ((!isSgPointerType(fromType) && !isSgArrayType(fromType) && !isSgTypeString(fromType))
           || (!isSgPointerType(toType)   && !isSgArrayType(toType)   && !isSgTypeString(toType)))
        {
          return Value::evalCastExp(fromVal, fromType, toType);
        }
     else
        {
          SgType *toTargType = SageInterface::getElementType(toType);
          fromVal->dereference()->reinterpret(toTargType);
          return fromVal;
        }
   }

ValueP StackFrame::evalAssignOp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalExpr(lhs)->assign(evalExpr(rhs), lhs->get_type(), rhs->get_type());
   }

static size_t sageOffsetOf(ValueP val, SgClassType *apt, SgSymbol *field)
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
     SgNode *fieldDecl;
     if (SgVariableSymbol *vs = isSgVariableSymbol(field))
          fieldDecl = vs->get_declaration();
     else if (SgClassSymbol *cs = isSgClassSymbol(field))
          fieldDecl = cs->get_declaration()->get_definingDeclaration();
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

ValueP offsetValueOf(ValueP lhs, SgClassType *lhsApt, SgSymbol *symbol)
   {
     SgClassDefinition *symcdef = isSgClassDefinition(symbolScope(symbol));
     ROSE_ASSERT(symcdef != NULL);
     SgClassDeclaration *symcdecl = symcdef->get_declaration();
     ValueP base;
     bool isDirect = false;
     if (isBaseClassOf(symcdecl, static_cast<SgClassDeclaration *>(lhsApt->get_declaration())))
        {
          base = lhs;
          isDirect = true;
        }
     else
        {
          SgSymbol *symcsym = symcdecl->search_for_symbol_from_symbol_table();
          base = offsetValueOf(lhs, lhsApt, symcsym);
        }
     ValueP val;
     switch (symcdecl->get_class_type())
        {
          case SgClassDeclaration::e_union:
             val = ValueP(new UnionDiscriminantValue(base, symbol));
             break;
          default:
             size_t offset = sageOffsetOf(isDirect ? lhs : ValueP(), symcdecl->get_type(), symbol);
             val = ValueP(new OffsetValue(base, offset));
             break;
        }
#if 0
     if (symbol->get_type()->stripTypedefsAndModifiers()->variantT() == V_SgArrayType)
        {
          val = ValueP(new PointerValue(val, PTemp, val->owner));
        }
#endif
     return val;
   }

ValueP NonstaticFieldValue::evalDotExp(ValueP lhs, SgType *lhsApt) const
   {
#if 0
     SgClassType *lhsAptCls = isSgClassType(lhsApt->stripTypedefsAndModifiers());
     ROSE_ASSERT(lhsAptCls != NULL);
     size_t offset = sageOffsetOf(lhs, lhsAptCls, symbol);
     return ValueP(new OffsetValue(lhs, offset));
#endif
     SgClassType *lhsAptCls = isSgClassType(lhsApt->stripTypedefsAndModifiers());
     ROSE_ASSERT(lhsAptCls != NULL);
     return offsetValueOf(lhs, lhsAptCls, symbol);
   }

string UnionDiscriminantValue::show() const
   {
     return "{" + discriminant->get_name().getString() + " -> " + base->show() + "}";
   }

size_t UnionDiscriminantValue::forwardValidity() const
   {
     return base->forwardValidity();
   }

size_t UnionDiscriminantValue::backwardValidity() const
   {
     return base->backwardValidity();
   }

/* This will lead to O(2^n) calls to setDiscriminantAtOffset, more than we need but this
   ensures that the calls arrive in the correct order. */
void UnionDiscriminantValue::setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant)
   {
     base->setDiscriminantAtOffset(0, this->discriminant);
     base->setDiscriminantAtOffset(offset, discriminant);
   }

const_ValueP UnionDiscriminantValue::fieldAtOffset(size_t offset) const
   {
     base->setDiscriminant(discriminant);
     return base->fieldAtOffset(offset);
   }

ValueP UnionDiscriminantValue::fieldAtOffset(size_t offset)
   {
     base->setDiscriminant(discriminant);
     return base->fieldAtOffset(offset);
   }

void UnionDiscriminantValue::forEachSubfield(FieldVisitor &visit, long offset)
   {
     base->setDiscriminant(discriminant);
     base->forEachSubfield(visit, offset);
   }

string BuiltinFunctionValue::show() const { return "<<built-in function " + functionName() + ">>"; }

ValueP BuiltinFunctionValue::primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
   {
     throw InterpError("Cannot assign to a function!");
   }

size_t BuiltinFunctionValue::forwardValidity() const { return 1; }

bool BuiltinFunctionValue::valid() const { return true; }

void BuiltinFunctionValue::destroy()
   {
     throw InterpError("Cannot destroy a function");
   }

string memcpyFnValue::functionName() const { return "memcpy"; }

ValueP memcpyFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 3)
        {
          throw InterpError("Function memcpy called with incorrect arity");
        }
     ValueP destPtr = args[0], srcPtr = args[1], nVal = args[2];
     ValueP destArea = destPtr->dereference(), srcArea = srcPtr->dereference();
     
     size_t n = getConcreteValueF<size_t>::f(nVal->prim());

     struct PrimCopy : FieldVisitor
        {
          ValueP srcArea;
          size_t n;
          PrimCopy(ValueP srcArea, size_t n) : srcArea(srcArea), n(n) {}
          void operator()(long offset, ValueP destPrim)
             {
               if (offset < 0 || offset >= long(n)) return;
               ValueP srcOffset (new OffsetValue(srcArea, offset));
               SgType *destType = destPrim->defaultType();
               destPrim->assign(srcOffset, destType, destType);
             }
        };

     PrimCopy pc(srcArea, n);
     destArea->forEachPrim(pc);

     return ValueP();
   }

string memcmpFnValue::functionName() const { return "memcmp"; }

ValueP memcmpFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 3)
        {
          throw InterpError("Function memcmp called with incorrect arity");
        }
     ValueP s1Ptr = args[0], s2Ptr = args[1], nVal = args[2];
     ValueP s1Area = s1Ptr->dereference(), s2Area = s2Ptr->dereference();
     
     size_t n = getConcreteValueF<size_t>::f(nVal);

     struct PrimComp : FieldVisitor
        {
          ValueP s2Area;
          size_t n;
          int result;
          PrimComp(ValueP s2Area, size_t n) : s2Area(s2Area), n(n), result(0) {}
          void operator()(long offset, ValueP s1Prim)
             {
               if (result != 0) return;
               if (offset < 0 || offset >= long(n)) return;
               ValueP s2Offset (new OffsetValue(s2Area, offset));
               SgType *s1Type = s1Prim->defaultType();
               if (s1Prim->evalLessThanOp(s2Offset, s1Type, s1Type)->getConcreteValueInt())
                  {
                    result = -1;
                  }
               else if (s1Prim->evalGreaterThanOp(s2Offset, s1Type, s1Type)->getConcreteValueInt())
                  {
                    result = 1;
                  }
             }
        };

     PrimComp pc(s2Area, n);
     s1Area->forEachPrim(pc);

     return ValueP(new IntValue(pc.result, PTemp, owner));
   }

string memsetFnValue::functionName() const { return "memset"; }

ValueP memsetFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 3)
        {
          throw InterpError("Function memset called with incorrect arity");
        }
     ValueP sPtr = args[0], cVal = args[1], nVal = args[2];
     ValueP sArea = sPtr->dereference();

     size_t n = getConcreteValueF<size_t>::f(nVal->prim());

     interpMemset(owner, sArea, cVal, fnType->get_arguments()[1], n);
     return ValueP();
   }

void interpMemset(StackFrameP owner, ValueP sArea, ValueP cVal, SgType *cValType, size_t n)
   {
     struct PrimSet : FieldVisitor
        {
          size_t n;
          ValueP cVal;
          SgType *cValType;
          StackFrameP owner;

          PrimSet(size_t n, ValueP cVal, SgType *cValType, StackFrameP owner) :
                  n(n),
                  cVal(cVal),
                  cValType(cValType),
                  owner(owner) {}

          void operator()(long offset, ValueP destPrim)
             {
               if (offset < 0 || offset >= long(n)) return;
               SgType *destType = destPrim->defaultType();
               ValueP newVal = owner->newValue(destType, PTemp);
               newVal->evalCastExp(cVal, cValType, destType);
               destPrim->assign(newVal, destType, destType);
             }
        };

     PrimSet ps(n, cVal, cValType, owner);
     sArea->forEachPrim(ps);
   }

string mallocFnValue::functionName() const { return "malloc"; }

ValueP mallocFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 1)
        {
          throw InterpError("Function malloc called with incorrect arity");
        }
     ValueP sizeVal = args[0];
     size_t size = getConcreteValueF<size_t>::f(sizeVal->prim());

     ValueP area (new DynamicValue(size, PHeap, owner));
     ValueP areaPtr (new PointerValue(area, PTemp, owner));

     return areaPtr;
   }

string reallocFnValue::functionName() const { return "realloc"; }

ValueP reallocFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 2)
        {
          throw InterpError("Function realloc called with incorrect arity");
        }
     ValueP ptrVal = args[0], sizeVal = args[1];
     size_t size = getConcreteValueF<size_t>::f(sizeVal->prim());

     ValueP area = ptrVal->unsafeDereference();
     if (area == ValueP())
        {
          ValueP newArea (new DynamicValue(size, PHeap, owner));
          ValueP newAreaPtr (new PointerValue(newArea, PTemp, owner));

          return newAreaPtr;
        }
     if (size == 0)
        {
          area->destroy();
          return ValueP(new PointerValue(ValueP(), PTemp, owner));
        }

     area->realloc(size);
     return ptrVal;
   }

string freeFnValue::functionName() const { return "free"; }

ValueP freeFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 1)
        {
          throw InterpError("Function free called with incorrect arity");
        }
     ValueP ptr = args[0];

     ptr->dereference()->destroy();
     return ValueP();
   }

string __builtin_expectFnValue::functionName() const { return "__builtin_expect"; }

ValueP __builtin_expectFnValue::call(SgFunctionType *fnType, const std::vector<ValueP> &args) const
   {
     if (args.size() != 2)
        {
          throw InterpError("Function __builtin_expect called with incorrect arity");
        }
     ValueP result = args[0];
     return result;
   }

ValueP StackFrame::evalDotExp(SgExpression *lhs, SgExpression *rhs)
   {
     return evalExpr(rhs, false)->evalDotExp(evalExpr(lhs), lhs->get_type());
   }

ValueP StackFrame::evalArrowExp(SgExpression *lhs, SgExpression *rhs)
   {
     SgPointerType *lhsPtrType = isSgPointerType(lhs->get_type()->stripTypedefsAndModifiers());
     ROSE_ASSERT(lhsPtrType != NULL);
     return evalExpr(rhs, false)->evalDotExp(evalExpr(lhs)->dereference(), lhsPtrType->get_base_type());
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
     SgFunctionType *fnType = isSgFunctionType(fn->get_type()->stripTypedefsAndModifiers());
     vector<ValueP> argVals = evalExprListExp(args, &fnType->get_arguments());
     try
        {
          return fnVal->call(fnType, argVals);
        }
     catch (InterpError &ie)
        {
          ie.callStack.push_back(InterpError::Frame(shared_from_this(), NULL));
          throw;
        }
   }

ValueP Value::call(SgFunctionType *fnType, const vector<ValueP> &argVals) const
   {
     StackFrameP down = createStackFrame();
     return down->interpFunction(argVals);
   }

vector<ValueP> StackFrame::evalExprListExp(SgExprListExp *exprList, SgTypePtrList *types)
   {
     const SgExpressionPtrList &exprs = exprList->get_expressions();
     SgTypePtrList::const_iterator typeI;
     if (types)
        {
          if (types->size() == 0)
             {
            // K&R prototype, no casting possible
               types = 0;
             }
          else
             {
            // Won't work for varargs
            // ROSE_ASSERT(exprs.size() == types->size());
               typeI = types->begin();
             }
        }
     vector<ValueP> exprVals;
     for (SgExpressionPtrList::const_iterator exprI = exprs.begin(); exprI != exprs.end(); ++exprI)
        {
          ValueP exprVal = evalExpr(*exprI);
          if (types)
             {
               ROSE_ASSERT(typeI != types->end());
               if ((*typeI)->variantT() == V_SgTypeEllipse)
                  {
                    types = 0;
                  }
               else if ((*exprI)->get_type() != *typeI)
                  {
                    ValueP castExprVal = newValue(*typeI, PTemp);
                    exprVal = castExprVal->evalCastExp(exprVal, (*exprI)->get_type(), *typeI);
                  }
               ++typeI;
             }
          exprVals.push_back(exprVal);
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
     ValueP opdVal = evalExpr(opd); \
     return opdVal->eval##opname(opd->get_type()); \
   }

#define DEFINE_STACKFRAME_EVALBINOP(op,opname) \
ValueP StackFrame::eval##opname(SgExpression *lhs, SgExpression *rhs) \
   { \
     ValueP lhsVal = evalExpr(lhs), rhsVal = evalExpr(rhs); \
     return lhsVal->eval##opname(rhsVal, lhs->get_type(), rhs->get_type()); \
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
     if (!lhsPrim->valid() || lhsPrim->getConcreteValueInt() == sctype) return lhsEval; \
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

ValueP StackFrame::evalCommaOpExp(SgExpression *lhs, SgExpression *rhs)
   {
     evalExpr(lhs);
     return evalExpr(rhs);
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
          case V_SgCommaOpExp: return evalCommaOpExp(lhs, rhs);
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
     return strArray;
   }

string valueToString(ValueP strArray)
   {
     stringstream ss;
     for (size_t i = 0;; ++i)
        {
          ValueP chr = strArray->primAtOffset(i);
          char c = chr->getConcreteValueChar();
          if (c == 0)
               break;
          ss << c;
        }
     return ss.str();
   }

ValueP StackFrame::evalStringVal(SgStringVal *strVal)
   {
     // TODO: store strArray as an attribute of the SgStringVal
     return stringToValue(strVal->get_value());
   }

ValueP StackFrame::evalConditionalExp(SgConditionalExp *condExp)
   {
     ValueP cond = evalExpr(condExp->get_conditional_exp());
     if (cond->prim()->getConcreteValueInt() != 0)
        {
          return evalExpr(condExp->get_true_exp());
        }
     else
        {
          return evalExpr(condExp->get_false_exp());
        }
   }

ValueP StackFrame::evalVarArgStartOp(SgVarArgStartOp *vaStart)
   {
     SgExpression *vaListExp = vaStart->get_lhs_operand();
     ValueP vaListVal = evalExpr(vaListExp);
     ValueP myVaList (new VAListValue(vaList, PTemp, shared_from_this()));
     vaListVal->assign(myVaList, vaListExp->get_type(), vaListExp->get_type());
     return ValueP();
   }

ValueP StackFrame::evalVarArgOp(SgVarArgOp *vaArg)
   {
     SgExpression *vaListExp = vaArg->get_operand_expr();
     ValueP vaListVal = evalExpr(vaListExp);
     return vaListVal->evalVarArgOp(vaArg->get_expression_type());
   }

ValueP StackFrame::evalVarArgEndOp(SgVarArgEndOp *vaEnd)
   {
     SgExpression *vaListExp = vaEnd->get_operand_expr();
     ValueP vaListVal = evalExpr(vaListExp);
     vaListVal->evalVarArgEndOp();
     return ValueP();
   }

ValueP StackFrame::evalExpr(SgExpression *expr, bool arrPtrConv)
   {
     ValueP val;
     if (SgUnaryOp *unOp = isSgUnaryOp(expr)) val = evalUnaryOp(unOp);
     else if (SgBinaryOp *binOp = isSgBinaryOp(expr)) val = evalBinaryOp(binOp);
     else switch (expr->variantT())
        {
          case V_SgBoolValExp: val = evalPrimExpr<BoolValue, SgBoolValExp>(expr); break;
          case V_SgCharVal: val = evalPrimExpr<CharValue, SgCharVal>(expr); break;
          case V_SgDoubleVal: val = evalPrimExpr<DoubleValue, SgDoubleVal>(expr); break;
          case V_SgEnumVal: val = evalPrimExpr<IntValue, SgEnumVal>(expr); break;
          case V_SgFloatVal: val = evalPrimExpr<FloatValue, SgFloatVal>(expr); break;
          case V_SgIntVal: val = evalPrimExpr<IntValue, SgIntVal>(expr); break;
          case V_SgLongDoubleVal: val = evalPrimExpr<LongDoubleValue, SgLongDoubleVal>(expr); break;
          case V_SgLongIntVal: val = evalPrimExpr<LongIntValue, SgLongIntVal>(expr); break;
          case V_SgLongLongIntVal: val = evalPrimExpr<LongLongIntValue, SgLongLongIntVal>(expr); break;
          case V_SgShortVal: val = evalPrimExpr<ShortValue, SgShortVal>(expr); break;
          case V_SgStringVal: val = evalStringVal(isSgStringVal(expr)); break;
          case V_SgUnsignedCharVal: val = evalPrimExpr<UnsignedCharValue, SgUnsignedCharVal>(expr); break;
          case V_SgUnsignedIntVal: val = evalPrimExpr<UnsignedIntValue, SgUnsignedIntVal>(expr); break;
          case V_SgUnsignedLongLongIntVal: val = evalPrimExpr<UnsignedLongLongIntValue, SgUnsignedLongLongIntVal>(expr); break;
          case V_SgUnsignedLongVal: val = evalPrimExpr<UnsignedLongValue, SgUnsignedLongVal>(expr); break;
          case V_SgUnsignedShortVal: val = evalPrimExpr<UnsignedShortValue, SgUnsignedShortVal>(expr); break;
          case V_SgVarRefExp: val = evalVarRefExp(isSgVarRefExp(expr)); break;
          case V_SgFunctionRefExp: val = evalFunctionRefExp(isSgFunctionRefExp(expr)->get_symbol()); break;
          case V_SgMemberFunctionRefExp: val = evalMemberFunctionRefExp(isSgMemberFunctionRefExp(expr)->get_symbol()); break;
          case V_SgFunctionCallExp: val = evalFunctionCallExp(isSgFunctionCallExp(expr)); break;
          case V_SgThisExp: val = evalThisExp(); break;
          case V_SgConditionalExp: val = evalConditionalExp(isSgConditionalExp(expr)); break;
          case V_SgNullExpression: val = ValueP(); break;
          case V_SgVarArgStartOp: val = evalVarArgStartOp(isSgVarArgStartOp(expr)); break;
          case V_SgVarArgOp: val = evalVarArgOp(isSgVarArgOp(expr)); break;
          case V_SgVarArgEndOp: val = evalVarArgEndOp(isSgVarArgEndOp(expr)); break;
          // Liao 3/28/2013, not sure if this is entirely correct.
          //case V_SgAggregateInitializer: aggregateInitialize (val, isSgAggregateInitializer(expr), NULL, false); break;
          default: throw InterpError("unhandled expression " + expr->class_name() + " encountered");
        }
     // The SAGE IR does not explicitly represent array-to-pointer conversion.
     // C99 6.3.2.1 para 3 gives the rules for this type of conversion, and
     // arrPtrConv is set in those circumstances requiring a conversion.
     if (arrPtrConv)
        {
          SgType *exprType = expr->get_type()->stripTypedefsAndModifiers();
          VariantT exprTypeV = exprType->variantT();
          if (exprTypeV == V_SgArrayType || exprTypeV == V_SgTypeString)
             {
               return ValueP(new PointerValue(val, PTemp, shared_from_this()));
             }
        }
     return val;
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
       BasicBlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgBasicBlock *bb, SgStatementPtrList::iterator *stmtI = NULL) : BlockStackFrame(up, sf, bb), stmts(bb->get_statements()), stmtI(stmtI ? *stmtI : stmts.begin()) {}

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

void StackFrame::assignInitialize(ValueP var, SgAssignInitializer *assignInit, SgType *varApt, bool isStatic)
   {
     VariantT initTypeV = assignInit->get_type()->stripTypedefsAndModifiers()->variantT();
     bool arrPtrConv = true;
     if (initTypeV == V_SgArrayType || initTypeV == V_SgTypeString)
          arrPtrConv = false;
     var->assign(evalExpr(assignInit->get_operand(), arrPtrConv), varApt, assignInit->get_operand()->get_type());
   }

void StackFrame::ctorInitialize(ValueP var, SgConstructorInitializer *ctorInit, SgType *varApt, bool isStatic)
   {
     vector<ValueP> argVals = evalExprListExp(ctorInit->get_args(), &ctorInit->get_declaration()->get_type()->get_arguments());
     StackFrameP down = newStackFrame(isSgFunctionSymbol(ctorInit->get_declaration()->search_for_symbol_from_symbol_table()), var);
     down->interpFunction(argVals);
   }

void StackFrame::aggregateInitialize(ValueP var, SgAggregateInitializer *aggInit, SgType *varApt, bool isStatic)
   {
     SgType *initType = aggInit->get_type()->stripTypedefsAndModifiers();
     SgExpressionPtrList &inits = aggInit->get_initializers()->get_expressions();
     if (SgArrayType *at = isSgArrayType(initType))
        {
          SgType *elemType = at->get_base_type();
          const StructLayoutInfo &info = typeLayout(elemType);
          size_t ofs = 0, defaultElemCount;
          if (SgUnsignedLongVal *idxUL = isSgUnsignedLongVal(at->get_index()))
               defaultElemCount = idxUL->get_value();
          else if (SgIntVal *idxI = isSgIntVal(at->get_index()))
               defaultElemCount = idxI->get_value();
          else
               throw InterpError("Array size not constant");
          for (SgExpressionPtrList::const_iterator i = inits.begin(); i != inits.end(); ++i, ofs += info.size, --defaultElemCount)
             {
               SgInitializer *init = isSgInitializer(*i);
               ROSE_ASSERT(init != NULL);
               // TODO: handle SgDesignatedInitializers
               ValueP ofsVal (new OffsetValue(var, ofs));
               initialize(var->fieldAtOffset(ofs), init, elemType, isStatic);
             }
          for (; defaultElemCount != 0; ofs += info.size, --defaultElemCount)
             {
               defaultInitialize(var->fieldAtOffset(ofs), isStatic);
             }
        }
     else if (SgClassType *ct = isSgClassType(initType))
        {
          if (isSgClassDeclaration(ct->get_declaration())->get_class_type() == SgClassDeclaration::e_union)
             {
            // initialize "the first named member of a union" (C99 6.7.8p17)
               SgVariableSymbol *vs = firstNamedMember(ct);
               ROSE_ASSERT(vs != NULL);
               var->setDiscriminant(vs);
               SgInitializer *init = isSgInitializer(inits.front());
               initialize(var->fieldAtOffset(0), init, vs->get_type(), isStatic);
             }
          else
             {
               const StructLayoutInfo &info = typeLayout(ct);
               vector<StructLayoutEntry>::const_iterator si = info.fields.begin();
               for (SgExpressionPtrList::const_iterator ei = inits.begin(); si != info.fields.end() && ei != inits.end(); ++si, ++ei)
                  {
                    while (si != info.fields.end() && si->decl == NULL) ++si;
                    if (si == info.fields.end()) break;
                    SgInitializer *init = isSgInitializer(*ei);
                    ROSE_ASSERT(init != NULL);
                 // TODO: handle SgDesignatedInitializers
                    initialize(var->fieldAtOffset(si->byteOffset), init, fieldType(si->decl), isStatic);
                  }
               for(; si != info.fields.end(); ++si)
                  {
                    while (si != info.fields.end() && si->decl == NULL) ++si;
                    if (si == info.fields.end()) break;
                    defaultInitialize(var->fieldAtOffset(si->byteOffset), isStatic);
                  }
             }
        }
   }

void Value::assignDefault()
   {
     throw InterpError("This type of value does not support being assigned to the default value.");
   }

void PointerValue::assignDefault()
   {
     target = ValueP();
     isValid = true;
   }

void CompoundValue::assignDefault()
   {
     for (fieldMap_t::const_iterator fieldI = fields.begin(); fieldI != fields.end(); ++fieldI)
        {
          fieldI->second->assignDefault();
        }
   }

void UnionValue::assignDefault()
   {
     setDiscriminant(firstNamedMember(unionType));
     storage->assignDefault();
   }

void StackFrame::defaultInitialize(ValueP var, bool isStatic)
   {
  // TODO: look up a 0-arg ctor for this object

     if (isStatic)
        {
          var->assignDefault();
        }
   }

void StackFrame::initialize(ValueP var, SgInitializer *init, SgType *varApt, bool isStatic)
   {
     if (init)
        {
          switch (init->variantT())
             {
               case V_SgAssignInitializer: assignInitialize(var, isSgAssignInitializer(init), varApt, isStatic); break;
               case V_SgConstructorInitializer: ctorInitialize(var, isSgConstructorInitializer(init), varApt, isStatic); break;
               case V_SgAggregateInitializer: aggregateInitialize(var, isSgAggregateInitializer(init), varApt, isStatic); break;
               default: cerr << "StackFrame::initialize: unrecognized initializer " << init->class_name() << endl; break;
             }
        }
     else
        {
          defaultInitialize(var, isStatic);
        }
   }

ValueP StackFrame::evalInitializedName(SgInitializedName *var, blockScopeVars_t &blockScope, varBindings_t &varBindings, bool isStatic)
   {
     SgVariableSymbol *sym = isSgVariableSymbol(var->search_for_symbol_from_symbol_table());
     ROSE_ASSERT(sym != NULL);
     SgInitializer *init = var->get_initializer();
     SgType *varType = var->get_type();
     if (isSgArrayType(varType) && init) // in this case, var may have an incomplete array type
                                         // which we need to replace with the complete type
                                         // from the initializer
        {
          varType = init->get_type();
        }
     ValueP binding = newValue(varType, PStack);
     initialize(binding, init, var->get_type(), isStatic);
     varBindings[sym] = binding;
     blockScope.push_back(sym);
     return binding;
   }

ValueP StackFrame::evalVariableDecl(SgVariableDeclaration *vdec, blockScopeVars_t &blockScope, varBindings_t &varBindings, bool isStatic)
   {
     SgInitializedNamePtrList &vars = vdec->get_variables();
     ValueP rv;
     for (SgInitializedNamePtrList::iterator varI = vars.begin(); varI != vars.end(); ++varI)
        {
          rv = evalInitializedName(*varI, blockScope, varBindings, isStatic);
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
            return cond->getConcreteValueInt() ? body : NULL;
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

struct StackFrame::SwitchBlockStackFrame : public BlockStackFrame
     {
       SwitchBlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgScopeStatement *scope) :
               BlockStackFrame(up, sf, scope) {}

       BlockStackFrameP doBreak()
          {
            destroyScope();
            return up;
          }

     };

/*
 * Note: case statements may appear in places other than the top-level basic block of the
 * switch statement (e.g. see Duff's device).  This code does not handle such cases.
 */
void StackFrame::evalSwitchStatement(SgSwitchStatement *switchStmt, BlockStackFrameP &curFrame)
   {
     curFrame = BlockStackFrameP(new SwitchBlockStackFrame(curFrame, shared_from_this(), switchStmt));
     ValueP selectorVal = evalStmtAsExpr(switchStmt->get_item_selector(), curFrame->scopeVars);
     unsigned long long selectorULL = selectorVal->getConcreteValueUnsignedLongLong();

     SgBasicBlock *bodyBB = isSgBasicBlock(switchStmt->get_body());
     if (bodyBB == NULL)
        throw InterpError("StackFrame::evalSwitchStatement: expected a SgBasicBlock body, found a " + bodyBB->class_name());

     SgStatementPtrList &stmts = bodyBB->get_statements();
     SgStatementPtrList::iterator nextStmtI = stmts.end(), defaultStmtI = stmts.end();
     for (SgStatementPtrList::iterator curStmtI = stmts.begin();
          nextStmtI == stmts.end() && curStmtI != stmts.end(); ++curStmtI)
        {
          SgStatement *curStmt = *curStmtI;
          switch (curStmt->variantT())
             {
               case V_SgDefaultOptionStmt: defaultStmtI = curStmtI; break;
               case V_SgCaseOptionStmt:
                 {
                   SgCaseOptionStmt *optStmt = isSgCaseOptionStmt(curStmt);
                   ValueP keyVal = evalExpr(optStmt->get_key());
                   if (keyVal->getConcreteValueUnsignedLongLong() == selectorULL)
                      {
                        nextStmtI = curStmtI;
                      }
                   break;
                 }
               default: break;
             }
        }
     if (nextStmtI == stmts.end())
          nextStmtI = defaultStmtI;

     if (nextStmtI != stmts.end())
        {
          curFrame = BlockStackFrameP(new BasicBlockStackFrame(curFrame, shared_from_this(), bodyBB, &nextStmtI));
        }
   }

void StackFrame::evalCaseOptionStmt(SgCaseOptionStmt *caseOptStmt, BlockStackFrameP &curFrame)
   {
     evalStmt(caseOptStmt->get_body(), curFrame);
   }

void StackFrame::evalDefaultOptionStmt(SgDefaultOptionStmt *defaultOptStmt, BlockStackFrameP &curFrame)
   {
     evalStmt(defaultOptStmt->get_body(), curFrame);
   }

void StackFrame::evalStmt(SgStatement *stmt, BlockStackFrameP &curFrame)
   {
     blockScopeVars_t &blockScope = curFrame->scopeVars;
     try
        {
          switch (stmt->variantT())
             {
               case V_SgBasicBlock: evalBasicBlock(isSgBasicBlock(stmt), curFrame); break;
               case V_SgVariableDeclaration: evalVariableDecl(isSgVariableDeclaration(stmt), blockScope, localVarBindings, isSgVariableDeclaration(stmt)->get_declarationModifier().get_storageModifier().isStatic()); break;
               case V_SgExprStatement: evalExpr(isSgExprStatement(stmt)->get_expression()); break;
               case V_SgIfStmt: evalIfStmt(isSgIfStmt(stmt), curFrame); break;
               case V_SgWhileStmt: evalWhileStmt(isSgWhileStmt(stmt), curFrame); break;
               case V_SgDoWhileStmt: evalDoWhileStmt(isSgDoWhileStmt(stmt), curFrame); break;
               case V_SgForStatement: evalForStatement(isSgForStatement(stmt), curFrame); break;
               case V_SgSwitchStatement: evalSwitchStatement(isSgSwitchStatement(stmt), curFrame); break;
               case V_SgCaseOptionStmt: evalCaseOptionStmt(isSgCaseOptionStmt(stmt), curFrame); break;
               case V_SgDefaultOptionStmt: evalDefaultOptionStmt(isSgDefaultOptionStmt(stmt), curFrame); break;
               case V_SgBreakStmt: curFrame = curFrame->doBreak(); break;
               case V_SgContinueStmt: curFrame = curFrame->doContinue(); break;
               case V_SgReturnStmt: evalReturnStmt(isSgReturnStmt(stmt), curFrame); break;
               default: cerr << "StackFrame::evalStmt: unhandled statement " << stmt->sage_class_name() << " encountered, skipping" << endl;
             }
        }
     catch (InterpError &ie)
        {
          InterpError::Frame &frame = ie.callStack[ie.callStack.size()-1];
          if (!frame.first)
             {
               frame.first = shared_from_this();
             }
          if (frame.second == NULL)
             {
               frame.second = stmt;
             }
          throw;
        }
     if (interp()->trace)
        {
          cout << "after evalStmt(" << stmt->unparseToString() << "): ";
#if 1
          if (thisBinding.get())
             {
               cout << "thisBinding = " << thisBinding->show() << ", ";
             }
          cout << "localVarBindings = ";
          dumpBindings(cout, localVarBindings);
#if 1
          cout << ", globalVarBindings = ";
          dumpBindings(cout, interp()->globalVarBindings);
#endif
#endif
          cout << endl;
        }
   }

ValueP StackFrame::evalStmtAsExpr(SgStatement *stmt, blockScopeVars_t &blockScope)
   {
     switch (stmt->variantT())
        {
          case V_SgVariableDeclaration: return evalVariableDecl(isSgVariableDeclaration(stmt), blockScope, localVarBindings);
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
     if (defDecl == NULL)
        {
          throw InterpError("Undefined function: " + funSym->get_name().getString());
        }
     language = SageInterface::getEnclosingFileNode(defDecl)->get_outputLanguage();
     SgFunctionDefinition *def = defDecl->get_definition();
     ROSE_ASSERT(def != NULL);
     BlockStackFrameP fnBlock (new BlockStackFrame(BlockStackFrameP(), shared_from_this(), def));
     SgInitializedNamePtrList &formalParams = defDecl->get_args();
  // The below assert won't work for varargs
  // ROSE_ASSERT(formalParams.size() == actualParams.size());
          SgInitializedNamePtrList::iterator fpI = formalParams.begin();
     for (vector<ValueP>::const_iterator apI = actualParams.begin();
                     apI != actualParams.end();
                     ++fpI, ++apI)
        {
          ROSE_ASSERT(fpI != formalParams.end());
          SgVariableSymbol *fpSym = isSgVariableSymbol((*fpI)->get_symbol_from_symbol_table());
          SgType *fpType = (*fpI)->get_type();
          if (fpSym != NULL) // fpSym is NULL if the parameter variable is anonymous
             {
               if (isSgArrayType(fpType->stripTypedefsAndModifiers()))
                  {
                    localVarBindings[fpSym] = (*apI)->dereference();
                  }
               else
                  {
                    localVarBindings[fpSym] = (*apI)->copy(fpType, PStack, shared_from_this(), CParam);
                    fnBlock->scopeVars.push_back(fpSym);
                  }
             }
          else if (fpType->variantT() == V_SgTypeEllipse)
             {
               vaList = VAListP(new VAList);
               do {
                    vaList->push_back(*apI);
               } while (++apI != actualParams.end());
               break;
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
                       initialize(thisBinding, ctor->get_initializer(), ctor->get_type());
                       break;
                    case SgInitializedName::e_data_member:
                          {
                            SgSymbol *memSym = ctor->search_for_symbol_from_symbol_table();
                            ROSE_ASSERT(memSym != NULL);
                            SgVariableSymbol *memVarSym = isSgVariableSymbol(memSym);
                            ROSE_ASSERT(memVarSym != NULL);
                            ValueP offsetVal = offsetValueOf(thisBinding, memDefDecl->get_associatedClassDeclaration()->get_type(), memVarSym);
                            initialize(offsetVal, ctor->get_initializer(), ctor->get_type());
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

void StackFrame::initializeGlobals(SgScopeStatement *scope)
   {
     blockScopeVars_t dummyScope;
     SgDeclarationStatementPtrList &decls = scope->getDeclarationList();
     for (SgDeclarationStatementPtrList::iterator i = decls.begin(); i != decls.end(); ++i)
        {
          SgDeclarationStatement *decl = *i;
          if (SgVariableDeclaration *varDecl = isSgVariableDeclaration(decl))
             {
               if (!varDecl->get_declarationModifier().get_storageModifier().isExtern())
                    evalVariableDecl(varDecl, dummyScope, interp()->globalVarBindings, true);
               if (interp()->trace)
                  {
                    cout << "after initializeGlobals(" << varDecl->unparseToString() << "): ";
                    cout << "globalVarBindings = ";
                    dumpBindings(cout, interp()->globalVarBindings);
                    cout << endl;
                  }
             }
          else if (SgNamespaceDeclarationStatement *nsDecl = isSgNamespaceDeclarationStatement(decl))
             {
               initializeGlobals(nsDecl->get_definition());
             }
          // TODO: find static variable declarations within function bodies
        }
   }

void StackFrame::initializeGlobals(SgProject *prj)
   {
     SgFilePtrList &fileList = prj->get_fileList();
     for (SgFilePtrList::iterator file = fileList.begin(); file != fileList.end(); ++file)
        {
          SgSourceFile *srcFile = isSgSourceFile(*file);
          ROSE_ASSERT(srcFile != NULL);
          initializeGlobals(srcFile->get_globalScope());
        }
   }

StackFrame::~StackFrame() {}

void Interpretation::prePrimAssign(ValueP lhs, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) {}

void Interpretation::parseCommandLine(vector<string> &args)
   {
     trace = CommandlineProcessing::isOption(args, "-interp:", "trace", true);
     errorTrace = CommandlineProcessing::isOption(args, "-interp:", "errorTrace", true);
   }

Interpretation::~Interpretation()
   {
     if (_builtinFns != NULL)
          delete _builtinFns;
   }

SgFunctionSymbol *prjFindGlobalFunction(const SgProject *prj, const SgName &fnName)
   {
     const SgFilePtrList &files = prj->get_fileList();
     for (SgFilePtrList::const_iterator i = files.begin(); i != files.end(); ++i)
        {
          SgSourceFile *srcFile = isSgSourceFile(*i);
          if (srcFile == NULL) continue;
          SgGlobal *global = srcFile->get_globalScope();
          SgFunctionSymbol *funSym = global->lookup_function_symbol(fnName);
          if (funSym != NULL) return funSym;
        }
     return NULL;
   }

};
