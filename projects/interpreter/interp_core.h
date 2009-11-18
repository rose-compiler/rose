#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

namespace Interp {

enum Position
   {
     PHeap,
     PStack,
     PGlob,
     PTemp
   };

// Context in which a new value is being created
enum Context
   {
     COther,
     CParam, // parameter passing
     CField  // field of a struct of union
   };

class StackFrame;
class Value;

typedef boost::shared_ptr<StackFrame> StackFrameP;
typedef boost::shared_ptr<Value> ValueP;
typedef boost::shared_ptr<const Value> const_ValueP;

typedef std::vector<ValueP> VAList;
typedef boost::shared_ptr<VAList> VAListP;

class InterpError
   {
     public:
     typedef std::pair<StackFrameP, SgStatement *> Frame;

     std::vector<Frame> callStack;
     std::string err;

     InterpError(std::string err);
     void dumpCallStack(std::ostream &out);
   };

#define FOREACH_UNARY_PRIMOP(unop) \
        unop(-,MinusOp) \
        unop(+,UnaryAddOp)

#define FOREACH_NOFP_UNARY_PRIMOP(nofpunop) \
        nofpunop(~,BitComplementOp) \

#define FOREACH_BINARY_PRIMOP(binop) \
        binop(+,AddOp,PlusAssignOp) \
        binop(-,SubtractOp,MinusAssignOp) \
        binop(*,MultiplyOp,MultAssignOp) \
        binop(/,DivideOp,DivAssignOp)

#define FOREACH_NOFP_BINARY_PRIMOP(nofpbinop) \
        nofpbinop(%,ModOp,ModAssignOp) \
        nofpbinop(^,BitXorOp,XorAssignOp) \
        nofpbinop(&,BitAndOp,AndAssignOp) \
        nofpbinop(|,BitOrOp,IorAssignOp)

/*
#define FOREACH_ASSIGN_PRIMOP(assignop) \
        assignop(+,PlusAssignOp) \
        assignop(-,MinusAssignOp) \
        assignop(*,MultAssignOp) \
        assignop(/,DivAssignOp) \
        assignop(%,ModAssignOp) \
        assignop(^,XorAssignOp) \
        assignop(&,AndAssignOp) \
        assignop(|,OrAssignOp) \
        assignop(>>,) \
        assignop(<<)
*/

#define FOREACH_SHIFT_PRIMOP(shiftop) \
        shiftop(<<,LshiftOp,LshiftAssignOp) \
        shiftop(>>,RshiftOp,RshiftAssignOp)

#define FOREACH_BOOL_UNARY_PRIMOP(boolunop) \
        boolunop(!,NotOp)

#define FOREACH_BOOL_BINARY_PRIMOP(boolbinop) \
        boolbinop(<,LessThanOp) \
        boolbinop(>,GreaterThanOp) \
        boolbinop(==,EqualityOp) \
        boolbinop(!=,NotEqualOp) \
        boolbinop(<=,LessOrEqualOp) \
        boolbinop(>=,GreaterOrEqualOp)

#define FOREACH_BOOL_SC_BINARY_PRIMOP(scboolbinop) \
        scboolbinop(&&,AndOp,false) \
        scboolbinop(||,OrOp,true)

class FieldVisitor
   {
     public:
     virtual void operator()(long offset, ValueP prim) = 0;
     virtual ~FieldVisitor();
   };

Value *mkBoolean(bool b, Position pos, StackFrameP owner);
Value *mkBoolean(Position pos, StackFrameP owner);

/* Note: throughout the Intepreter code, apt stands for "apparent type".  This means
   the type that the compiler believes that the given value is.  If an incorrect type
   is used, this is an error, even if we are able to carry out the operation at
   runtime! */

class Value : public boost::enable_shared_from_this<Value>
   {

     public:
     virtual std::string show() const = 0;

     Position pos;
     StackFrameP owner;

#define GDECLARE_VIRTUAL_UNOP_FN(op,opname,cv) \
     virtual ValueP eval##opname(SgType *apt) cv;
#define DECLARE_VIRTUAL_UNOP_FN(op,opname) GDECLARE_VIRTUAL_UNOP_FN(op,opname,const)

#define GDECLARE_VIRTUAL_BINOP_FN(op,opname,cv) \
     virtual ValueP eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) cv;
#define DECLARE_VIRTUAL_BINOP_FN(op,opname) GDECLARE_VIRTUAL_BINOP_FN(op,opname,const)
#define DECLARE_VIRTUAL_BINOP_FNS(op,opname,opassignname) \
     GDECLARE_VIRTUAL_BINOP_FN(op,opname,const) \
     GDECLARE_VIRTUAL_BINOP_FN(op,opassignname,)
#define DECLARE_VIRTUAL_SC_BINOP_FN(op,opname,sctype) DECLARE_VIRTUAL_BINOP_FN(op,opname)

     FOREACH_UNARY_PRIMOP(          DECLARE_VIRTUAL_UNOP_FN)
     FOREACH_NOFP_UNARY_PRIMOP(     DECLARE_VIRTUAL_UNOP_FN)
     FOREACH_BINARY_PRIMOP(         DECLARE_VIRTUAL_BINOP_FNS)
     FOREACH_NOFP_BINARY_PRIMOP(    DECLARE_VIRTUAL_BINOP_FNS)
     FOREACH_SHIFT_PRIMOP(          DECLARE_VIRTUAL_BINOP_FNS)
     FOREACH_BOOL_UNARY_PRIMOP(     DECLARE_VIRTUAL_UNOP_FN)
     FOREACH_BOOL_BINARY_PRIMOP(    DECLARE_VIRTUAL_BINOP_FN)
     // FOREACH_BOOL_SC_BINARY_PRIMOP( DECLARE_VIRTUAL_SC_BINOP_FN)

#undef GDECLARE_VIRTUAL_UNOP_FN
#undef DECLARE_VIRTUAL_UNOP_FN
#undef GDECLARE_VIRTUAL_BINOP_FN
#undef DECLARE_VIRTUAL_BINOP_FN
#undef DECLARE_VIRTUAL_BINOP_FNS
#undef DECLARE_VIRTUAL_SC_BINOP_FN

     virtual ValueP evalPrefixPlusPlusOp(SgType *apt);
     virtual ValueP evalPostfixPlusPlusOp(SgType *apt);
     virtual ValueP evalPrefixMinusMinusOp(SgType *apt);
     virtual ValueP evalPostfixMinusMinusOp(SgType *apt);

     /*! Returns true if Value is valid (i.e. defined; can be read from). */
     virtual bool valid() const = 0;

     /*! assign is equivalent to the = operator. */
     ValueP assign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);

     /*!  primAssign is caled by assign for primitive (non-compound) assignments */
     virtual ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) = 0;

     /*! copy is equivalent to the copy constructor.  */
     ValueP copy(SgType *apt, Position pos, StackFrameP owner, Context ctx = COther) const;

     /*!  primCopy is called by copy for primitive (non-compound) copies.
          The default implementation is based on primAsssign. */
     virtual ValueP primCopy(SgType *apt, Position pos, StackFrameP owner, Context ctx = COther) const;

     /*! Returns the number of valid bytes in this Value looking forward (typically
         the sizeof the Value */
     virtual size_t forwardValidity() const = 0;

     /*! Returns the number of valid bytes "behind" this Value.  Usually zero, but for
         OffsetValues this would be the offset. */
     virtual size_t backwardValidity() const;

     /*! Returns a new object with the same type as this type, if possible.  However
         it DOES NOT copy the object's contents (the new object is left uninitialised).  This
         is to allow the user to make a copy using either assign or copy as appropriate. */
     // virtual ValueP another(Position pos, StackFrameP owner) const = 0;

     /*! Destroys an object as though it were to go out of scope, or "delete' were invoked.
         Because the destructor will be called, this does not do the same thing as the free
         function call. */
     virtual void destroy() = 0;

     virtual ValueP fieldAtOffset(size_t offset);
     virtual const_ValueP fieldAtOffset(size_t offset) const;

     ValueP primAtOffset(size_t offset);
     const_ValueP primAtOffset(size_t offset) const;

     ValueP prim() { return primAtOffset(0); }
     const_ValueP prim() const { return primAtOffset(0); }

     virtual void forEachSubfield(FieldVisitor &visit, long offset = 0);
     virtual void forEachPrim(FieldVisitor &visit, long offset = 0);

     std::vector<SgClassType *> dynamicTypesAtOffset(size_t offset) const;
     virtual void buildDynamicTypesAtOffset(size_t offset, std::vector<SgClassType *> &types) const;
     virtual bool hasDynamicTypeAtOffset(SgClassType *t, size_t offset) const;

     //! Dereferences the Value.  It may return a NULL Value (ValueP()) if
     //  the pointer is NULL.
     virtual ValueP unsafeDereference() const;

     //! Dereferences the Value.  Guaranteed to return a valid Value (throws an exception
     //  when dereferencing a NULL pointer).
     ValueP dereference() const;

     virtual SgFunctionSymbol *getStaticFunctionSymbol() const;
     virtual void setStaticFunctionSymbol(SgFunctionSymbol *sym);

     virtual SgMemberFunctionSymbol *getNonstaticFunctionSymbol() const;
     virtual void setNonstaticFunctionSymbol(SgMemberFunctionSymbol *sym);

     /*! Creates a new stack frame representing a call to this function object (only works
         for bona fide function objects). */
     virtual StackFrameP createStackFrame() const;

     /*! Calls this function with the given arguments (only works for bona fide function
         objects).  If an interpretation needs to provide its own implementation for a
         particular function it can override evalFunctionRefExp to return a specialised
         Value for SgFunctionRefs to that function, which overrides this call method. */
     virtual ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &argVals) const;

     /*! This function is actually used for both ArrowExps and DotExps.  Note that the
         parameter here is the lhs (i.e. "*this"); this allows us to handle fields
         distinctly from methods through the virtual function. */
     virtual ValueP evalDotExp(ValueP lhs, SgType *lhsApt) const;

     /*! This function implements the va_arg operation for va_list values. */
     virtual ValueP evalVarArgOp(SgType *apt);

     /*! This function implements the va_end operation for va_list values. */
     virtual void evalVarArgEndOp();

     /*! These functions return concrete representations where possible.  They could also
         be used to implement casting. */
     virtual bool getConcreteValueBool() const;
     virtual char getConcreteValueChar() const;
     virtual double getConcreteValueDouble() const;
     virtual float getConcreteValueFloat() const;
     virtual int getConcreteValueInt() const;
     virtual long double getConcreteValueLongDouble() const;
     virtual long int getConcreteValueLong() const;
     virtual long long int getConcreteValueLongLong() const;
     virtual short getConcreteValueShort() const;
     virtual unsigned char getConcreteValueUnsignedChar() const;
     virtual unsigned int getConcreteValueUnsignedInt() const;
     virtual unsigned long long int getConcreteValueUnsignedLongLong() const;
     virtual unsigned long getConcreteValueUnsignedLong() const;
     virtual unsigned short getConcreteValueUnsignedShort() const;
 
     /*! This function is different from the other eval... functions in that it is called
         on the target value with the source value as a parameter.  The rationale is
         that extracting the needed information from the source value is type-specific
         functionality. */
     virtual ValueP evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType);

     /*! Returns a type which closely resembles the given Value's type.  To be used only
         when the correct type is not known, and it is not possible to know the type
         (i.e. from information in the AST). */
     virtual SgType *defaultType() const;

     /*! Sets the discriminant of the union with the given field at the given offset to the
         given field of the union, if different from the current discriminant.  If the
         discriminant is changed, this will normally cause a reset of most fields in
         the UnionValue (but there are some special cases). */
     virtual void setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant);
     void setDiscriminant(SgSymbol *discriminant) { setDiscriminantAtOffset(0, discriminant); }

     /*! Sets this object to the default value.  The default value for an object is defined
         by C99 6.7.8p10.  This method is special in that it only needs to be implemented
         by "real" values, rather than passthrough values such as OffsetValue. */
     virtual void assignDefault();

     /*! Reinterprets this Value to (an array of) the given type.  This is called when a
         pointer to the Value (particularly a void pointer returned by malloc) is cast. */
     virtual void reinterpret(SgType *newType);

     /*! Performs a reallocation operation on this region of memory, as though the realloc
         function was called with an argument being a pointer to this region. */
     virtual void realloc(size_t newSize);

     //! Retrieves the eventual target and offset of this value.  The intention is that
     //  target and offset can act as a unique pair for pointer comparison and pointer
     //  arithmetic.
     virtual void targetOffset(const_ValueP &target, size_t &offset) const;

     Value(Position pos, StackFrameP owner);
     virtual ~Value();

   };

#define GDECLARE_UNOP_FN(op,opname,cv) \
     ValueP eval##opname(SgType *apt) cv;
#define DECLARE_UNOP_FN(op,opname) GDECLARE_UNOP_FN(op,opname,const)

#define GDECLARE_BINOP_FN(op,opname,cv) \
     ValueP eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) cv;
#define DECLARE_BINOP_FN(op,opname) GDECLARE_BINOP_FN(op,opname,const)
#define DECLARE_BINOP_FNS(op,opname,opassignname) \
     GDECLARE_BINOP_FN(op,opname,const) \
     GDECLARE_BINOP_FN(op,opassignname,)
#define DECLARE_SC_BINOP_FN(op,opname,sctype) DECLARE_BINOP_FN(op,opname)

#define DECLARE_ALL_PRIMOPS \
     FOREACH_UNARY_PRIMOP(          DECLARE_UNOP_FN) \
     FOREACH_NOFP_UNARY_PRIMOP(     DECLARE_UNOP_FN) \
     FOREACH_BINARY_PRIMOP(         DECLARE_BINOP_FNS) \
     FOREACH_NOFP_BINARY_PRIMOP(    DECLARE_BINOP_FNS) \
     FOREACH_SHIFT_PRIMOP(          DECLARE_BINOP_FNS) \
     FOREACH_BOOL_UNARY_PRIMOP(     DECLARE_UNOP_FN) \
     FOREACH_BOOL_BINARY_PRIMOP(    DECLARE_BINOP_FN)

/* TODO: introduce an abstraction for show() and validity here */
class BasePrimValue : public Value
   {
     protected:
     bool isValid;

     public:
     BasePrimValue(Position pos, StackFrameP owner, bool valid) : Value(pos, owner), isValid(valid) {}

     bool valid() const;
     void destroy();
   };


/*! Represents a pointer or reference.  If this pointer references an offset into another
    structure, it will point to an OffsetValue.  If this is a null pointer, target will be
    null (i.e. ValueP()). */
class PointerValue : public BasePrimValue
     {
       protected:
            ValueP target;
            ValueP adjustedOffset(SgType *apt, size_t offset) const;

       public:
            PointerValue(Position pos, StackFrameP owner) : BasePrimValue(pos, owner, false) {}
            PointerValue(ValueP target, Position pos, StackFrameP owner) : BasePrimValue(pos, owner, true), target(target) {}

            ValueP getTarget() const { return target; }
            void setTarget(ValueP target) { this->target = target; this->isValid = true; }

            size_t forwardValidity() const;

            ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
            std::string show() const;

            ValueP evalAddOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;
            ValueP evalSubtractOp(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const;

            ValueP evalPrefixPlusPlusOp(SgType *apt);
            ValueP evalPrefixMinusMinusOp(SgType *apt);

            ValueP evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType);

            FOREACH_BOOL_BINARY_PRIMOP(DECLARE_BINOP_FN)

            ValueP unsafeDereference() const;

            SgType *defaultType() const;

            void assignDefault();
     };

class VAListValue : public BasePrimValue
     {
       VAListP vaList;
       VAList::const_iterator vaListPos;

       public:
       VAListValue(Position pos, StackFrameP owner) : BasePrimValue(pos, owner, false) {}
       VAListValue(VAListP vaList, Position pos, StackFrameP owner) : BasePrimValue(pos, owner, true), vaList(vaList), vaListPos(vaList->begin()) {}
       std::string show() const;
       ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
       size_t forwardValidity() const;
       ValueP evalVarArgOp(SgType *apt);
       void evalVarArgEndOp();

     };

/*! This is the base class for values which are based on SgSymbols - i.e. functions, or 
    non-static fields.*/
class SymbolValue : public Value
     {

       protected:
            virtual SgSymbol *getSymbol() const = 0;

       public:
            SymbolValue(Position pos, StackFrameP owner) : Value(pos, owner) {}

            bool valid() const;
            std::string show() const;
            ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
            size_t forwardValidity() const;
            void destroy();

     };

/*! Repreents static functions, i.e. global functions or static member functions. */
class StaticFunctionValue : public SymbolValue
     {

       protected:
            SgFunctionSymbol *symbol;
            SgSymbol *getSymbol() const;

       public:
            StaticFunctionValue(SgFunctionSymbol *symbol, Position pos, StackFrameP owner) : SymbolValue(pos, owner), symbol(symbol) {}

            SgFunctionSymbol *getStaticFunctionSymbol() const;
            void setStaticFunctionSymbol(SgFunctionSymbol *sym);

            StackFrameP createStackFrame() const;


     };

/*! Represents non-static member functions.  These cannot be called directly without a "this"
    parameter. */
class NonstaticFunctionValue : public SymbolValue
     {

       protected:
            SgMemberFunctionSymbol *symbol;
            SgSymbol *getSymbol() const;

       public:
            NonstaticFunctionValue(SgMemberFunctionSymbol *symbol, Position pos, StackFrameP owner) : SymbolValue(pos, owner), symbol(symbol) {}

            SgMemberFunctionSymbol *getNonstaticFunctionSymbol() const;
            void setNonstaticFunctionSymbol(SgMemberFunctionSymbol *sym);

            ValueP evalDotExp(ValueP lhs, SgType *lhsApt) const;

     };

/*! Represents a non-static function with associated "this" parameter.  Formed when an
    SgDotExp, SgDotStarOp, SgArrowExp or SgArrowStarOp is encountered with a
    method rhs. */
class ThisNonstaticFunctionValue : public SymbolValue
     {

       protected:
            ValueP pThis;
            SgType *thisApt;
            SgMemberFunctionSymbol *symbol;
            SgSymbol *getSymbol() const;

       public:
            ThisNonstaticFunctionValue(ValueP pThis, SgType *thisApt, SgMemberFunctionSymbol *symbol, Position pos, StackFrameP owner) : SymbolValue(pos, owner), pThis(pThis), thisApt(thisApt), symbol(symbol) {}

            StackFrameP createStackFrame() const;
            std::string show() const;

     };

class NonstaticFieldValue : public SymbolValue
     {

       protected:
            SgVariableSymbol *symbol;
            SgSymbol *getSymbol() const;

       public:
            NonstaticFieldValue(SgVariableSymbol *symbol, Position pos, StackFrameP owner) : SymbolValue(pos, owner), symbol(symbol) {}

            ValueP evalDotExp(ValueP lhs, SgType *lhsApt) const;

     };

/*! Represents a function with an internal implementation inside the interpreter.  The
    core interpreter uses this to implement (part of) the C standard library; extensions
    can use it to implement whatever functions they need. */
class BuiltinFunctionValue : public Value
   {
     public:
     BuiltinFunctionValue(Position pos, StackFrameP owner) : Value(pos, owner) {}

     virtual std::string functionName() const = 0;

     std::string show() const;

     ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const = 0;
     ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
     size_t forwardValidity() const;

     bool valid() const;
     void destroy();

   };

class memcpyFnValue : public BuiltinFunctionValue
     {
       public:
            memcpyFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

class memcmpFnValue : public BuiltinFunctionValue
     {
       public:
            memcmpFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

void interpMemset(StackFrameP owner, ValueP sArea, ValueP cVal, SgType *cValType, size_t n);

class memsetFnValue : public BuiltinFunctionValue
     {
       public:
            memsetFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

class mallocFnValue : public BuiltinFunctionValue
     {
       public:
            mallocFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

ValueP interpMalloc(StackFrameP owner, size_t size);

class reallocFnValue : public BuiltinFunctionValue
     {
       public:
            reallocFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

class freeFnValue : public BuiltinFunctionValue
     {
       public:
            freeFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

class __builtin_expectFnValue : public BuiltinFunctionValue
     {
       public:
            __builtin_expectFnValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}
            std::string functionName() const;
            ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     };

/*! This class implements the arithmetic operations and primAssign using the primitive
    value that is accessible using prim.  Compound values and offset values inherit from this. */
class BaseCompoundValue : public Value
     {
       public:

            BaseCompoundValue(Position pos, StackFrameP owner) : Value(pos, owner) {}

            ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);

            DECLARE_ALL_PRIMOPS

            ValueP evalPrefixPlusPlusOp(SgType *apt);
            ValueP evalPostfixPlusPlusOp(SgType *apt);
            ValueP evalPrefixMinusMinusOp(SgType *apt);
            ValueP evalPostfixMinusMinusOp(SgType *apt);

            ValueP evalVarArgOp(SgType *apt);
            void evalVarArgEndOp();

            void forEachSubfield(FieldVisitor &visit, long offset = 0) = 0;
            void forEachPrim(FieldVisitor &visit, long offset = 0);

            ValueP unsafeDereference() const;

            bool getConcreteValueBool() const;
            char getConcreteValueChar() const;
            double getConcreteValueDouble() const;
            float getConcreteValueFloat() const;
            int getConcreteValueInt() const;
            long double getConcreteValueLongDouble() const;
            long int getConcreteValueLong() const;
            long long int getConcreteValueLongLong() const;
            short getConcreteValueShort() const;
            unsigned char getConcreteValueUnsignedChar() const;
            unsigned int getConcreteValueUnsignedInt() const;
            unsigned long long int getConcreteValueUnsignedLongLong() const;
            unsigned long getConcreteValueUnsignedLong() const;
            unsigned short getConcreteValueUnsignedShort() const;

            void setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant);

            bool valid() const;
     };

#undef GDECLARE_UNOP_FN
#undef DECLARE_UNOP_FN
#undef GDECLARE_BINOP_FN
#undef DECLARE_BINOP_FN
#undef DECLARE_BINOP_FNS
#undef DECLARE_SC_BINOP_FN

/*! The purpose of the CompoundValue is to represent ANY contiguous
    section of valid memory as a set of fields (including arrays, which
    are treated as sequences of fields with uniform type).  In this way,
    we are able to handle most forms of pointer arithmetic. */
class CompoundValue : public BaseCompoundValue
     {
       friend class DynamicValue;

       public:
       typedef std::map<size_t, ValueP> fieldMap_t;

       private:
       //! Stores the field (or array element) at this offset
       fieldMap_t fields;

       //! Stores the total size of this compound (i.e. malloc param or sizeof(dynamicType))
       size_t size;

#if 0
       // This comment makes no sense with recent changes and should be removed:

       // This must be a multimap.  Consider the case where a structure appears
       // at the beginning of an enclosing structure.  As there are no vtbls, both
       // structures will have the same offset.  If vtbls are ever added to the
       // LayoutGenerators, this can change to a map because although two structres
       // can appear at the same offset (e.g. structs without vtbls), this map is
       // only used for dynamic_cast which of course is only available for polymorphic
       // classes (i.e. classes with vtbls)
#endif
       //! Stores the dynamic type of the object
       SgClassType *dynamicType;

       public:
       /*! Constructor for a CompoundValue representing a struct. */
       CompoundValue(SgClassType *dynamicType, Position pos, StackFrameP owner);

       /*! Constructor for a CompoundValue representing a filled-in struct.
           Only for use in special cases (such as "copy"). */
       CompoundValue(SgClassType *dynamicType, fieldMap_t fields, Position pos, StackFrameP owner, size_t size = SIZE_MAX);

       /*! Constructor for a CompoundValue holding the underlying storage for an array. */
       CompoundValue(SgType *baseType, size_t index, Position pos, StackFrameP owner); 

       std::string show() const;

       void forEachSubfield(FieldVisitor &visit, long offset = 0);

       /*
       ValueP assign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
       ValueP copy(SgType *apt, Position pos, StackFrameP owner) const;
        */

       void destroy();

       ValueP fieldAtOffset(size_t offset);
       const_ValueP fieldAtOffset(size_t offset) const;

       void buildDynamicTypesAtOffset(size_t offset, std::vector<SgClassType *> &types) const;
       bool hasDynamicTypeAtOffset(SgClassType *t, size_t offset) const;

       size_t forwardValidity() const;

       void setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant);
       void assignDefault();
     };

/*! Represents an offset into another value (usually a CompoundValue), obtained using
    pointer arithmetic, field lookup or other means. */
class OffsetValue : public BaseCompoundValue
     {
       ValueP base;
       size_t offset;

       public:
       OffsetValue(ValueP base, size_t offset);

       std::string show() const;

       size_t forwardValidity() const;
       size_t backwardValidity() const;

       ValueP getBase() const { return base; }
       size_t getOffset() const { return offset; }

       ValueP fieldAtOffset(size_t offset);
       const_ValueP fieldAtOffset(size_t offset) const;

       void forEachSubfield(FieldVisitor &visit, long offset = 0);
       void destroy();

       void targetOffset(const_ValueP &target, size_t &offset) const;
     };

/*! UnionDiscriminantValue is to UnionValue as OffsetValue is to CompoundValue.  Calls to
    fieldAtOffset will set the UnionValue's discriminant. */
class UnionDiscriminantValue : public BaseCompoundValue
     {
       ValueP base;
       SgSymbol *discriminant;

       public:
       UnionDiscriminantValue(ValueP base, SgSymbol *discriminant) : BaseCompoundValue(base->pos, base->owner), base(base), discriminant(discriminant) {}

       std::string show() const;

       size_t forwardValidity() const;
       size_t backwardValidity() const;

       ValueP fieldAtOffset(size_t offset);
       const_ValueP fieldAtOffset(size_t offset) const;

       void forEachSubfield(FieldVisitor &visit, long offset = 0);
       void setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant);
       void destroy();
       void targetOffset(const_ValueP &target, size_t &offset) const;
     };

/*! UnionValues change their underlying storage in response to calls to setDiscriminant. */
class UnionValue : public BaseCompoundValue
     {
       /*! Type of this union (used to identify calls to setDiscriminant as belonging to
           this UnionValue (as opposed to a subfield of this UnionValue)). */
       SgClassType *unionType;

       /*! The current discriminant. */
       SgSymbol *currDiscriminant;

       /*! The underlying storage for this UnionValue. */ 
       ValueP storage;

       public:
       UnionValue(SgClassType *unionType, Position pos, StackFrameP owner) :
               BaseCompoundValue(pos, owner),
               unionType(unionType),
               currDiscriminant(NULL),
               storage() {}

       std::string show() const;
       size_t forwardValidity() const;
       void forEachSubfield(FieldVisitor &visit, long offset);
       ValueP fieldAtOffset(size_t offset);
       const_ValueP fieldAtOffset(size_t offset) const;
       void setDiscriminantAtOffset(size_t offset, SgSymbol *discriminant);
       void destroy();
       void assignDefault();
     };

/*! DynamicValue is the Value pointed to by the result of malloc().  It changes its underlying
    storage in response to calls to reinterpret(), which is called when the pointer is cast. */

class DynamicValue : public BaseCompoundValue
     {
       size_t size;
       
       /*! We need to be able to support the value being initialized
           (i.e. using memset or calloc) before it is reinterpreted.
           We therefore initialize the Value with a "canary" field
           which we test for validity upon reinterpretation.  The
           isCanary boolean indicates whether the canary is in place.

           A more robust approach would involve multiple canaries,
           one for each byte of the allocated region.  By scanning
           for defined canaries we would be able to determine exactly
           which regions the client program has initialized. */
       bool storageIsCanary;

       ValueP storage;
       SgType *storageType;

       public:
       DynamicValue(size_t size, Position pos, StackFrameP owner);

       std::string show() const;
       size_t forwardValidity() const;
       void forEachSubfield(FieldVisitor &visit, long offset);
       ValueP fieldAtOffset(size_t offset);
       const_ValueP fieldAtOffset(size_t offset) const;
       void reinterpret(SgType *newType);
       void destroy();
       void realloc(size_t newSize);
     };

class BasePrimTypeValue : public BasePrimValue
   {
     public:
     BasePrimTypeValue(Position pos, StackFrameP owner, bool valid) : BasePrimValue(pos, owner, valid) {}

     virtual bool isCorrectApparentType(SgType *apt) const = 0;
   };

/*! SgPrimTypeT<p>::t is the SgType associated with primitive values of type t */
template <typename T> struct SgPrimTypeT {};
template <> struct SgPrimTypeT<bool> { typedef SgTypeBool t; };
template <> struct SgPrimTypeT<char> { typedef SgTypeChar t; };
template <> struct SgPrimTypeT<double> { typedef SgTypeDouble t; };
template <> struct SgPrimTypeT<float> { typedef SgTypeFloat t; };
template <> struct SgPrimTypeT<int> { typedef SgTypeInt t; };
template <> struct SgPrimTypeT<long double> { typedef SgTypeLongDouble t; };
template <> struct SgPrimTypeT<long int> { typedef SgTypeLong t; };
template <> struct SgPrimTypeT<long long int> { typedef SgTypeLongLong t; };
template <> struct SgPrimTypeT<short> { typedef SgTypeShort t; };
template <> struct SgPrimTypeT<unsigned char> { typedef SgTypeUnsignedChar t; };
template <> struct SgPrimTypeT<unsigned int> { typedef SgTypeUnsignedInt t; };
template <> struct SgPrimTypeT<unsigned long long int> { typedef SgTypeUnsignedLongLong t; };
template <> struct SgPrimTypeT<unsigned long> { typedef SgTypeUnsignedLong t; };
template <> struct SgPrimTypeT<unsigned short> { typedef SgTypeUnsignedShort t; };

/*! getConcreteValueF<p>::f(const_ValueP) calls the virtual function in the getConcreteValue
    family that returns a primitive value of type p. */
template <typename T> struct getConcreteValueF {};
template <> struct getConcreteValueF<bool> { static bool f(const_ValueP v) { return v->getConcreteValueBool(); } };
template <> struct getConcreteValueF<char> { static char f(const_ValueP v) { return v->getConcreteValueChar(); } };
template <> struct getConcreteValueF<double> { static double f(const_ValueP v) { return v->getConcreteValueDouble(); } };
template <> struct getConcreteValueF<float> { static float f(const_ValueP v) { return v->getConcreteValueFloat(); } };
template <> struct getConcreteValueF<int> { static int f(const_ValueP v) { return v->getConcreteValueInt(); } };
template <> struct getConcreteValueF<long double> { static long double f(const_ValueP v) { return v->getConcreteValueLongDouble(); } };
template <> struct getConcreteValueF<long int> { static long int f(const_ValueP v) { return v->getConcreteValueLong(); } };
template <> struct getConcreteValueF<long long int> { static long long int f(const_ValueP v) { return v->getConcreteValueLongLong(); } };
template <> struct getConcreteValueF<short> { static short f(const_ValueP v) { return v->getConcreteValueShort(); } };
template <> struct getConcreteValueF<unsigned char> { static unsigned char f(const_ValueP v) { return v->getConcreteValueUnsignedChar(); } };
template <> struct getConcreteValueF<unsigned int> { static unsigned int f(const_ValueP v) { return v->getConcreteValueUnsignedInt(); } };
template <> struct getConcreteValueF<unsigned long long> { static unsigned long long int f(const_ValueP v) { return v->getConcreteValueUnsignedLongLong(); } };
template <> struct getConcreteValueF<unsigned long> { static unsigned long f(const_ValueP v) { return v->getConcreteValueUnsignedLong(); } };
template <> struct getConcreteValueF<unsigned short> { static unsigned short f(const_ValueP v) { return v->getConcreteValueUnsignedShort(); } };

template <typename PrimType>
class GenericPrimTypeValue : public BasePrimTypeValue
   {
     protected:
     PrimType v;

     bool isCorrectApparentType(SgType *apt) const
        {
          return (dynamic_cast<typename SgPrimTypeT<PrimType>::t *>(apt->stripTypedefsAndModifiers()) != NULL);
        }

     public:

     GenericPrimTypeValue(Position pos, StackFrameP owner) : BasePrimTypeValue(pos, owner, false) {}
     GenericPrimTypeValue(PrimType v, Position pos, StackFrameP owner) : BasePrimTypeValue(pos, owner, true), v(v) {}

     ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt)
        {
          /*
          if (!rhsPrim)
             {
               throw InterpError("Assignment between incompatible types");
             }
          if (rhsApt && !isCorrectApparentType(rhsApt))
             {
               throw InterpError("Incorrect apparent type for assignment");
             }
             */
          const_ValueP rhsPrim = rhs->prim();
          if (rhsPrim->valid())
               setConcreteValue(getConcreteValueF<PrimType>::f(rhsPrim));
          else
               isValid = false;
          return shared_from_this();
        }

     PrimType getConcreteValue() const
        {
          if (!isValid)
               throw InterpError("Attempt to retrieve undefined value!");
          return v;
        }

     bool getConcreteValueBool() const
        {
          return (bool)(getConcreteValue());
        }

     char getConcreteValueChar() const
        {
          return (char)(getConcreteValue());
        }

     double getConcreteValueDouble() const
        {
          return (double)(getConcreteValue());
        }

     float getConcreteValueFloat() const
        {
          return (float)(getConcreteValue());
        }

     int getConcreteValueInt() const
        {
          return (int)(getConcreteValue());
        }

     long double getConcreteValueLongDouble() const
        {
          return (long double)(getConcreteValue());
        }

     long int getConcreteValueLong() const
        {
          return (long int)(getConcreteValue());
        }

     long long int getConcreteValueLongLong() const
        {
          return (long long int)(getConcreteValue());
        }

     short getConcreteValueShort() const
        {
          return (short)(getConcreteValue());
        }

     unsigned char getConcreteValueUnsignedChar() const
        {
          return (unsigned char)(getConcreteValue());
        }

     unsigned int getConcreteValueUnsignedInt() const
        {
          return (unsigned int)(getConcreteValue());
        }

     unsigned long long int getConcreteValueUnsignedLongLong() const
        {
          return (unsigned long long int)(getConcreteValue());
        }

     unsigned long getConcreteValueUnsignedLong() const
        {
          return (unsigned long)(getConcreteValue());
        }

     unsigned short getConcreteValueUnsignedShort() const
        {
          return (unsigned short)(getConcreteValue());
        }

     void setConcreteValue(PrimType v)
        {
          this->v = v;
          isValid = true;
        }

     ValueP evalCastExp(ValueP fromVal, SgType *fromType, SgType *toType)
        {
          ValueP fromValPrim = fromVal->prim();
          if (fromValPrim->valid())
              setConcreteValue(getConcreteValueF<PrimType>::f(fromVal->prim()));
          else
              isValid = false;
          return shared_from_this();
        }

     std::string show() const
        {
          if (!isValid) return "<<undefined>>";
          std::stringstream ss;
          ss << v+0;
          // ss << std::hex << "0x" << v+0;
          return ss.str();
        }

     size_t forwardValidity() const { return sizeof(PrimType); }

     ValueP evalPrefixPlusPlusOp(SgType *apt)
        {
          ++v;
          return shared_from_this();
        }

     ValueP evalPrefixMinusMinusOp(SgType *apt)
        {
          v -= 1; /* v-- is not defined for bool, so we use this workaround. */
          return shared_from_this();
        }

     SgType *defaultType() const
        {
          return SgPrimTypeT<PrimType>::t::createType();
        }

     void assignDefault()
        {
          setConcreteValue(0);
        }

   };

#define DEFINE_VIRTUAL_GENERIC_UNOP_IMPL(op,opname,resultfn) \
     ValueP eval##opname(SgType *apt) const \
        { \
          if (!this->isCorrectApparentType(apt)) \
             { \
               throw InterpError("Operator " #op " applied to incorrect apparent type"); \
             } \
          if (this->valid()) \
               return ValueP(resultfn(op this->getConcreteValue(), PTemp, this->owner)); \
          else \
               return ValueP(resultfn(PTemp, this->owner)); \
        }

#define DEFINE_VIRTUAL_UNOP_IMPL(op,opname) DEFINE_VIRTUAL_GENERIC_UNOP_IMPL(op,opname,new this_type)
#define DEFINE_VIRTUAL_BOOL_UNOP_IMPL(op,opname) DEFINE_VIRTUAL_GENERIC_UNOP_IMPL(op,opname,mkBoolean)

#define DEFINE_VIRTUAL_UNOP_ERROR_IMPL(op,opname) \
     ValueP eval##opname(SgType *) const \
        { \
          throw InterpError("Unsupported operator " #op " invoked for floating point type"); \
        }

#define DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,resultfn,rhstype,rhsprimtype) \
     ValueP eval##opname(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt) const \
        { \
          const_ValueP rhsP = rhs->prim(); \
          const BasePrimTypeValue *rhsPrim = dynamic_cast<const BasePrimTypeValue *>(rhsP.get()); \
          if (!this->isCorrectApparentType(lhsApt) || (rhsPrim && !rhsPrim->isCorrectApparentType(rhsApt))) \
             { \
               throw InterpError("Operator " #op " applied to incorrect apparent type"); \
             } \
          if (this->valid() && rhs->valid()) \
               return ValueP(resultfn(this->getConcreteValue() op getConcreteValueF<rhsprimtype>::f(rhsP), PTemp, this->owner)); \
          else \
               return ValueP(resultfn(PTemp, this->owner)); \
        }

#define DEFINE_VIRTUAL_BINOP_ERROR_IMPL(op,opname,opassignname) \
     ValueP eval##opname(const_ValueP, SgType *, SgType *) const \
        { \
          throw InterpError("Unsupported operator " #op " invoked for floating point type"); \
        } \
     ValueP eval##opassignname(const_ValueP, SgType *, SgType *) \
        { \
          throw InterpError("Unsupported operator " #op "= invoked for floating point type"); \
        }

#if 0
#define DEFINE_VIRTUAL_BINOP_IMPL(op,opname,opassignname) \
        DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,new this_type,this_type,getConcreteValue) \
        DEFINE_VIRTUAL_ASSIGN_BINOP_IMPL(opname,opassignname)
#define DEFINE_VIRTUAL_BOOL_BINOP_IMPL(op,opname) DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,mkBoolean,this_type,getConcreteValue)
#define DEFINE_VIRTUAL_SHIFTOP_IMPL(op,opname,opassignname) \
        DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,new this_type,BasePrimTypeValue,getConcreteValueInt) \
        DEFINE_VIRTUAL_ASSIGN_BINOP_IMPL(opname,opassignname)
#else
#define DEFINE_VIRTUAL_BINOP_IMPL(op,opname,opassignname) \
        DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,new this_type,this_type,PrimType)
#define DEFINE_VIRTUAL_BOOL_BINOP_IMPL(op,opname) DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,mkBoolean,this_type,PrimType)
#define DEFINE_VIRTUAL_SHIFTOP_IMPL(op,opname,opassignname) \
        DEFINE_VIRTUAL_GENERIC_BINOP_IMPL(op,opname,new this_type,BasePrimTypeValue,int)
#endif

template <typename PrimType>
class IntegralPrimTypeValue : public GenericPrimTypeValue<PrimType>
   {

     private:
     typedef IntegralPrimTypeValue<PrimType> this_type;

     public:
     IntegralPrimTypeValue(Position pos, StackFrameP owner) : GenericPrimTypeValue<PrimType>(pos, owner) {}
     IntegralPrimTypeValue(PrimType v, Position pos, StackFrameP owner) : GenericPrimTypeValue<PrimType>(v, pos, owner) {}

     FOREACH_UNARY_PRIMOP(       DEFINE_VIRTUAL_UNOP_IMPL)
     FOREACH_NOFP_UNARY_PRIMOP(  DEFINE_VIRTUAL_UNOP_IMPL)
     FOREACH_BINARY_PRIMOP(      DEFINE_VIRTUAL_BINOP_IMPL)
     FOREACH_NOFP_BINARY_PRIMOP( DEFINE_VIRTUAL_BINOP_IMPL)
     FOREACH_SHIFT_PRIMOP(       DEFINE_VIRTUAL_SHIFTOP_IMPL)
     FOREACH_BOOL_UNARY_PRIMOP(  DEFINE_VIRTUAL_BOOL_UNOP_IMPL)
     FOREACH_BOOL_BINARY_PRIMOP( DEFINE_VIRTUAL_BOOL_BINOP_IMPL)

   };

template <typename PrimType>
class FloatingPointPrimTypeValue : public GenericPrimTypeValue<PrimType>
   {

     private:
     typedef FloatingPointPrimTypeValue<PrimType> this_type;

     public:
     FloatingPointPrimTypeValue(Position pos, StackFrameP owner) : GenericPrimTypeValue<PrimType>(pos, owner) {}
     FloatingPointPrimTypeValue(PrimType v, Position pos, StackFrameP owner) : GenericPrimTypeValue<PrimType>(v, pos, owner) {}

     FOREACH_UNARY_PRIMOP(       DEFINE_VIRTUAL_UNOP_IMPL)
     FOREACH_NOFP_UNARY_PRIMOP(  DEFINE_VIRTUAL_UNOP_ERROR_IMPL)
     FOREACH_BINARY_PRIMOP(      DEFINE_VIRTUAL_BINOP_IMPL)
     FOREACH_NOFP_BINARY_PRIMOP( DEFINE_VIRTUAL_BINOP_ERROR_IMPL)
     FOREACH_SHIFT_PRIMOP(       DEFINE_VIRTUAL_BINOP_ERROR_IMPL)
     FOREACH_BOOL_UNARY_PRIMOP(  DEFINE_VIRTUAL_BOOL_UNOP_IMPL)
     FOREACH_BOOL_BINARY_PRIMOP( DEFINE_VIRTUAL_BOOL_BINOP_IMPL)

   };

#undef DEFINE_VIRTUAL_GENERIC_UNOP_IMPL
#undef DEFINE_VIRTUAL_UNOP_IMPL
#undef DEFINE_VIRTUAL_BOOL_UNOP_IMPL
#undef DEFINE_VIRTUAL_UNOP_ERROR_IMPL
#undef DEFINE_VIRTUAL_GENERIC_BINOP_IMPL
#undef DEFINE_VIRTUAL_BINOP_ERROR_IMPL
#undef DEFINE_VIRTUAL_ASSIGN_BINOP_IMPL
#undef DEFINE_VIRTUAL_BINOP_IMPL
#undef DEFINE_VIRTUAL_BOOL_BINOP_IMPL
#undef DEFINE_VIRTUAL_SHIFTOP_IMPL

typedef IntegralPrimTypeValue<bool> BoolValue;
typedef IntegralPrimTypeValue<char> CharValue;
typedef FloatingPointPrimTypeValue<double> DoubleValue;
typedef FloatingPointPrimTypeValue<float> FloatValue;
typedef IntegralPrimTypeValue<int> IntValue;
typedef FloatingPointPrimTypeValue<long double> LongDoubleValue;
typedef IntegralPrimTypeValue<long int> LongIntValue;
typedef IntegralPrimTypeValue<long long int> LongLongIntValue;
typedef IntegralPrimTypeValue<short> ShortValue;
typedef IntegralPrimTypeValue<unsigned char> UnsignedCharValue;
typedef IntegralPrimTypeValue<unsigned int> UnsignedIntValue;
typedef IntegralPrimTypeValue<unsigned long long int> UnsignedLongLongIntValue;
typedef IntegralPrimTypeValue<unsigned long> UnsignedLongValue;
typedef IntegralPrimTypeValue<unsigned short> UnsignedShortValue;

typedef std::map<SgVariableSymbol *, ValueP> varBindings_t;

class Interpretation
   {
     public:
     typedef std::map<std::string, ValueP> builtins_t;

     private:
     mutable builtins_t *_builtinFns;

     protected:
     virtual void registerBuiltinFns(builtins_t &builtins) const;

     public:
     bool trace, errorTrace;
     varBindings_t globalVarBindings;

     Interpretation();

     const builtins_t &builtinFns() const;

     virtual void parseCommandLine(std::vector<std::string> &args);

     /*! The purpose of this function is to allow the interpretation
         to hook any variable assignments that may occur in the program.
         For example, if the interpretation uses checkpointing, this
         allows the interpretation to save the state for a rollback. */
     virtual void prePrimAssign(ValueP lhs, const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);

     virtual ~Interpretation();
   };

void dumpBindings(std::ostream &out, const varBindings_t &bindings);

class StackFrame : public boost::enable_shared_from_this<StackFrame>
   {
     friend class Interpretation;
     friend class InterpError;

     Interpretation *currentInterp;

     protected:
     SgFunctionSymbol *funSym;

     /*! This is actually *this, not this. */
     ValueP thisBinding;

     varBindings_t localVarBindings;
     SgFile::outputLanguageOption_enum language;

     VAListP vaList;

     bool active;
     ValueP returnValue;

     template <typename PrimTypeValueT, class SgValExprT>
     ValueP evalPrimExpr(SgExpression *expr)
        {
          SgValExprT *pe = dynamic_cast<SgValExprT *>(expr);
          return ValueP(new PrimTypeValueT(pe->get_value(), PTemp, shared_from_this()));
        }

     void defaultInitialize(ValueP var, bool isStatic);

     void assignInitialize(ValueP var, SgAssignInitializer *assignInit, SgType *varApt, bool isStatic = false);
     void ctorInitialize(ValueP var, SgConstructorInitializer *ctorInit, SgType *varApt, bool isStatic = false);
     void aggregateInitialize(ValueP var, SgAggregateInitializer *aggInit, SgType *varApt, bool isStatic = false);
     void initialize(ValueP var, SgInitializer *init, SgType *varApt, bool isStatic = false);

     bool isGlobalVar(SgInitializedName *var);
     ValueP stringToValue(const std::string &str);
     ValueP evalStringVal(SgStringVal *strVal);
     virtual ValueP evalConditionalExp(SgConditionalExp *condExp);
     ValueP evalPointerDerefExp(SgExpression *opd);
     ValueP evalAddressOfOp(SgExpression *opd);
     ValueP evalCastExp(SgExpression *opd, SgType *toType);
     ValueP evalVarRefExp(ValueP symTabEntry, SgType *apt);
     ValueP evalVarRefExp(SgVarRefExp *vr);
     virtual ValueP evalFunctionRefExp(SgFunctionSymbol *sym);
     ValueP evalMemberFunctionRefExp(SgMemberFunctionSymbol *sym);
     ValueP evalAssignOp(SgExpression *lhs, SgExpression *rhs);
     ValueP evalFieldOrMethodAccess(ValueP lhsEval, SgClassType *lhsApt, SgExpression *rhs);
     ValueP evalDotExp(SgExpression *lhs, SgExpression *rhs);
     ValueP evalArrowExp(SgExpression *lhs, SgExpression *rhs);
     ValueP evalPntrArrRefExp(SgExpression *lhs, SgExpression *rhs);
     ValueP evalCommaOpExp(SgExpression *lhs, SgExpression *rhs);
     ValueP evalFunctionCallExp(SgFunctionCallExp *fnCall);
     std::vector<ValueP> evalExprListExp(SgExprListExp *exprList, SgTypePtrList *types = NULL);
     ValueP evalThisExp();
     ValueP evalVarArgStartOp(SgVarArgStartOp *vaStart);
     ValueP evalVarArgOp(SgVarArgOp *vaArg);
     ValueP evalVarArgEndOp(SgVarArgEndOp *vaEnd);

     ValueP evalPlusPlusOp(SgExpression *opd, SgUnaryOp::Sgop_mode mode);
     ValueP evalMinusMinusOp(SgExpression *opd, SgUnaryOp::Sgop_mode mode);

#define DECLARE_STACKFRAME_EVALUNOP(op,opname) \
     ValueP eval##opname(SgExpression *opd);

#define DECLARE_STACKFRAME_EVALBINOP(op,opname) \
     ValueP eval##opname(SgExpression *lhs, SgExpression *rhs);

#define DECLARE_STACKFRAME_EVALBINOPS(op,opname,opassignname) \
        DECLARE_STACKFRAME_EVALBINOP(op,opname) \
        DECLARE_STACKFRAME_EVALBINOP(op,opassignname)

#define DECLARE_STACKFRAME_SC_EVALBINOP(op,opname,sctype) \
        virtual DECLARE_STACKFRAME_EVALBINOP(op,opname)

     FOREACH_UNARY_PRIMOP(          DECLARE_STACKFRAME_EVALUNOP)
     FOREACH_NOFP_UNARY_PRIMOP(     DECLARE_STACKFRAME_EVALUNOP)
     FOREACH_BOOL_UNARY_PRIMOP(     DECLARE_STACKFRAME_EVALUNOP)
     FOREACH_BINARY_PRIMOP(         DECLARE_STACKFRAME_EVALBINOPS)
     FOREACH_NOFP_BINARY_PRIMOP(    DECLARE_STACKFRAME_EVALBINOPS)
     FOREACH_SHIFT_PRIMOP(          DECLARE_STACKFRAME_EVALBINOPS)
     FOREACH_BOOL_BINARY_PRIMOP(    DECLARE_STACKFRAME_EVALBINOP)
     FOREACH_BOOL_SC_BINARY_PRIMOP( DECLARE_STACKFRAME_SC_EVALBINOP)

#undef DECLARE_STACKFRAME_EVALUNOP
#undef DECLARE_STACKFRAME_EVALBINOP
#undef DECLARE_STACKFRAME_EVALBINOPS
#undef DECLARE_STACKFRAME_SC_EVALBINOP

     ValueP evalUnaryOp(SgUnaryOp *unOp);
     ValueP evalBinaryOp(SgBinaryOp *binOp);
     virtual ValueP evalExpr(SgExpression *expr, bool arrPtrConv = true);

     typedef std::vector<SgVariableSymbol *> blockScopeVars_t;

     struct BlockStackFrame;

     typedef boost::shared_ptr<BlockStackFrame> BlockStackFrameP;

     struct BlockStackFrame : boost::enable_shared_from_this<BlockStackFrame>
     {
       BlockStackFrame(BlockStackFrameP up, StackFrameP sf, SgScopeStatement *scope) : up(up), sf(sf), scope(scope) {}
       BlockStackFrameP up;
       StackFrameP sf;
       SgScopeStatement *scope;
       blockScopeVars_t scopeVars;
       virtual SgStatement *next();
       virtual void destroyScope();
       virtual BlockStackFrameP doContinue();
       virtual BlockStackFrameP doBreak();
       virtual ~BlockStackFrame();
     };

     struct BasicBlockStackFrame;
     struct LoopBlockStackFrame;
     struct SwitchBlockStackFrame;

     typedef std::vector<BlockStackFrameP> blockStack_t;

     void destroyScope(blockScopeVars_t &scope);

     void evalBasicBlock(SgBasicBlock *bb, BlockStackFrameP &curFrame);
     ValueP evalInitializedName(SgInitializedName *var, blockScopeVars_t &blockScope, varBindings_t &varBindings, bool isStatic = false);
     ValueP evalVariableDecl(SgVariableDeclaration *vdec, blockScopeVars_t &blockScope, varBindings_t &varBindings, bool isStatic = false);
     virtual void evalIfStmt(SgIfStmt *ifStmt, BlockStackFrameP &curFrame);
     void evalWhileStmt(SgWhileStmt *whileStmt, BlockStackFrameP &curFrame);
     void evalDoWhileStmt(SgDoWhileStmt *doWhileStmt, BlockStackFrameP &curFrame);
     void evalForInitStatement(SgForInitStatement *forInitStmt, blockScopeVars_t &blockScope);
     void evalForStatement(SgForStatement *forStmt, BlockStackFrameP &curFrame);
     void evalReturnStmt(SgReturnStmt *retStmt, BlockStackFrameP &curFrame);
     void evalSwitchStatement(SgSwitchStatement *switchStmt, BlockStackFrameP &curFrame);
     void evalCaseOptionStmt(SgCaseOptionStmt *caseOptStmt, BlockStackFrameP &curFrame);
     void evalDefaultOptionStmt(SgDefaultOptionStmt *defaultOptStmt, BlockStackFrameP &curFrame);
     void evalStmt(SgStatement *stmt, BlockStackFrameP &curFrame);
     ValueP evalStmtAsBool(SgStatement *stmt, blockScopeVars_t &blockScope);
     ValueP evalStmtAsExpr(SgStatement *stmt, blockScopeVars_t &blockScope);
     void mainEvalLoop(BlockStackFrameP &curFrame);

     virtual ValueP newArray(SgArrayType *at, Position pos, Context ctx);
     ValueP newClassValue(SgClassType *ct, Position pos);
     ValueP newTypedefValue(SgTypedefType *tt, Position pos, Context ctx);

     public:
     StackFrame(Interpretation *currentInterp, SgFunctionSymbol *funSym, ValueP thisBinding = ValueP()) : currentInterp(currentInterp), funSym(funSym), thisBinding(thisBinding), active(true) {}

     Interpretation *interp() const { return currentInterp; }

     void initializeGlobals(SgScopeStatement *scope);
     void initializeGlobals(SgProject *project);

     /*! Returns a new (undefined) value of the given type.  If isParam is true,
         the value is the callee's copy of a value passed as a parameter.  This
         is special cased in order to correctly handle passing of arrays. */
     virtual ValueP newValue(SgType *t, Position pos, Context ctx = COther);

     /*! Returns a new stack frame that is initialised with the given function
         and "this" binding.  This function must be overridden by any subclasses
         for stack frames below the top frame to be of the correct type. */
     virtual StackFrameP newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding = ValueP());

     SgFile::outputLanguageOption_enum get_language() { return language; }

     ValueP interpFunction(const std::vector<ValueP> &args);
     void dump();

     virtual ~StackFrame();

   };

SgFunctionSymbol *prjFindGlobalFunction(const SgProject *prj, const SgName &fnName);

std::string valueToString(ValueP strArray);

}; // namespace Interp

#endif
