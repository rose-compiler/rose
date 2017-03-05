// Originally based on https://outreach.scidac.gov/svn/rose/trunk@891
// src/midend/programTransformation/constantFolding/constantFolding.h

#ifndef CONSTANT_FOLDER_H
#define CONSTANT_FOLDER_H

#include <rose.h>
#include "rosetollvm/LLVMAstAttributes.h"

/**
 * \brief Provides classes to fold constant expressions.
 *
 * ConstantFolder makes the same assumptions about sizes, alignment, and
 * padding that the rest of RoseToLLVM does.  For example, they both
 * assume the same value for sizeof(long double), and they both assume
 * the same value for sizeof(void*).  For any tree that will be
 * translated by RoseToLLVM, it is not safe to employ a constant folder
 * that makes different assumptions, and so it is not necessarily safe
 * to depend on the constant folding originally performed by ROSE/EDG.
 *
 * ConstantFolder folds all expressions (or portions of expressions)
 * that are acceptable in C99 global initializers, which are a superset
 * of all other C99 constant expressions.  For simplicity, it assumes
 * that all objects whose addresses are referenced in an expression have
 * static storage duration.  This can result in some additional folding.
 * The ConstantFolder leaves all remaining expressions unfolded.
 *
 * The C99 specification of what must be permitted in global
 * initializers (and thus that must be folded by ConstantFolder) is a
 * bit cryptic when address constants are involved.  What ConstantFolder
 * permits (or rather, what it folds as opposed to leaving unfolded) is,
 * for now, based on our interpretation of C99 and our experience with
 * GCC 4.4.3, clang 2.8, and ROSE/EDG from
 * https://outreach.scidac.gov/svn/rose/trunk@949.
 *
 * The most unclear case involves casting addresses to integers.  C99
 * says that, when addresses are involved, a global initializer must
 * evaluate to, at most, an object address plus an integer constant
 * expression.  The word "evaluate" is the confusing point.  For
 * example, at the global scope:
 *
 *   int i;
 *   long l = 5 + (long)&i;
 *
 * Assuming long is the size of a pointer, we can imagine l's
 * initializer "evaluating" to the following expression, which is
 * equivalent:
 *
 *   (long)((char*)&i + 5)
 *
 * Thus, we have an object address (that's being treated as having a
 * different pointer type) plus an integer constant expression (and the
 * result is cast to a different type).  The compiler tools listed above
 * all accept such minor type conversions in a global initializer, so
 * ConstantFolder folds such expressions.  In contrast, most of the
 * tools above do not accept either of the following global
 * initializers, so ConstantFolder does not fold such expressions:
 *
 *   int i;
 *   long l = -(long)&i;
 *   int *p = (int*)5 + (long)&i;
 *
 * Permitting minor type conversions does not appear sufficient to
 * evaluate either of these initializers into an object address plus an
 * integer constant expression.  The first requires a unary minus
 * operation on a object address, and the second requires the object
 * address to be multiplied by sizeof(int).
 *
 * To implement the "evaluation" process, the ConstantFolder uses the
 * following rules.  An object address may be cast to an integer type
 * that is at least as large as a pointer.  (FIXME: However, if the
 * integer type is larger, it's not clear whether overflow in pointer
 * arithmetic must be mimicked.  Currently, it's not mimicked.)  The
 * only operations that may be performed on the result of such a cast
 * are operations that are permitted directly on an object address.
 * Other operations that are normally permitted on integers are not
 * permitted on the result.  From the above examples, adding an integer
 * is permitted, adding a pointer is not, and performing unary minus is
 * not.
 *
 * A notable exception to the above observations is that GCC supports
 * some extensions to C99 address constant expressions that the other
 * tools above and ConstantFolder do not support.  Specifically, GCC
 * sometimes permits constructs that are not always guaranteed to yield
 * a valid C99 constant expression as long as GCC can find a way to
 * logically remove those constructs during folding.  For example, GCC
 * does not permit "long l = -(long)&i;", but it does permit "long l =
 * -(-(long)&i);" because the minuses cancel one another.  The clang
 * manual (http://clang.llvm.org/docs/UsersManual.html#c_unsupp_gcc)
 * mentions that GCC also permits "long l = i - i;" even though "long l
 * = i;" is not permitted because "i" is not constant.  Clang, ROSE/EDG,
 * and ConstantFolder do not permit these examples.
 */
namespace ConstantFolder {
  /**
   * \brief
   *   Implements inherited attributes for the top-down traversal of
   *   ExpressionTraversal.
   */
  class InheritedAttribute {
    public:
      InheritedAttribute() : ignoreChildren_(false), ignore_(false) {}
      void inheritFrom(InheritedAttribute parent) {
        ignore_ = ignoreChildren_ = parent.ignoreChildren_;
      }
      void ignoreChildren() { ignoreChildren_ = true; }
      bool isIgnored() const { return ignore_; }
    private:
      bool ignoreChildren_;
      bool ignore_;
  };
  /**
   * \brief
   *   Implements synthesized attributes for the bottom-up traversals of
   *   ExpressionTraversal.
   *
   * This implements, for each node in the AST, a smart pointer for the
   * expression that results from folding the tree rooted at that node.
   * It can also store (1) a type to which to cast the expression after
   * folding is complete, (2) a byte offset to add to the expression
   * after folding is complete, and (3) a note that the parent node is
   * an \c SgAddressOfOp or \c SgDotExp (which, in our implementation,
   * is an address-of operation, an add, and a dereference) and that the
   * parent's address-of operation has already been folded into that
   * expression along with a child operation.  The cast type and byte
   * offset are useful only for folding address constant expressions (or
   * expressions that look like address constant expression when the
   * storage duration of the referenced object is not considered).  For
   * the case where an integer literal has been cast to a pointer, only
   * the cast type is useful, and it ceases to be useful if the result
   * is later cast back to an integer.
   *
   * The \c LLVMAstAtributes with which a \c SynthesizedAttribute is
   * constructed must be the same for every \c SynthesizedAttribute
   * associated with the same expression traversal.  It is used for
   * computing sizes, padding, and alignment of types, and it used for
   * managing ROSE AST attributes associated with that data.
   */
  class SynthesizedAttribute {
    public:
      /// Default constructor is required for
      /// \c AstTopDownBottomUpProcessing, and it is invoked.  However,
      /// the only place we should encounter a \c SynthesizedAttribute
      /// from the default constructor is when it is being overwritten
      /// by a \c SynthesizedAttribute we have constructed, or when the
      /// latter is temporarily overwritten by the former.  That's
      /// important because the former has no \c LLVMAstAttributes set,
      /// and the latter always should.  We should never invoke any
      /// method on the former though, so we always assert that.
      SynthesizedAttribute()
        : expr(NULL), refCount(NULL), castType(NULL), byteOffset(0),
          omitParentAddressOfOp(false), attributes(NULL)
      {
      }
      /// \brief Construct a SynthesizedAttribute with no expression,
      /// note, cast type, or byte offset.
      SynthesizedAttribute(LLVMAstAttributes *attributes_)
        : expr(NULL), refCount(NULL), castType(NULL), byteOffset(0),
          omitParentAddressOfOp(false), attributes(attributes_)
      {}
      /// \brief Store any expression, note, cast type, and byte offset
      /// from \c other into a new SynthesizedAttribute, and continue
      /// the memory management scheme from \c other.
      SynthesizedAttribute(SynthesizedAttribute const &other)
        : expr(NULL), refCount(NULL), castType(NULL), byteOffset(0),
          omitParentAddressOfOp(false), attributes(other.attributes)
      {
        *this = other;
      }
      /// \brief Safely overwrite any existing expression, note, cast
      /// type, and byte offset in \c this with those from \c other, and
      /// continue the memory management scheme from \c other.
      SynthesizedAttribute &operator=(
        SynthesizedAttribute const &other
      );
      ~SynthesizedAttribute() { clear(); }
      /// \brief Safely overwrite any existing expression in \c this
      /// with \c e without taking over the memory management of \c e.
      /// Remove any note, cast type, and byte offset.
      ///
      /// The caller must ensure that \c e lives as long as \c this and
      /// its copies.
      void setOldExpression(SgExpression *e) {
        ROSE2LLVM_ASSERT(attributes);
        clear(); ROSE2LLVM_ASSERT(e); expr = e;
      }
      /// \brief Safely overwrite any existing expression in \c this
      /// with \c e, and take responsibility for the memory of \c e.
      /// Remove any note, cast type, and byte offset.
      ///
      /// \c e will be destroyed using \c SageInterface::deepDelete once
      /// \c this and all copies of \c this are destroyed or their
      /// expressions are overwritten.
      void setNewExpression(SgExpression *e);
      /// \brief Get the stored expression or \c NULL if none.
      ///
      /// If the caller needs to ensure the expression outlives \c this,
      /// then the caller must either make a copy of the returned
      /// expression, or it must use an attribute copy function above
      /// or \c completeExpression below instead of using
      /// \c getExpression.
      SgExpression const *getExpression() const {
        ROSE2LLVM_ASSERT(attributes);
        return expr;
      }
      /// \brief Compute a new tree formed by a copy of the stored
      /// expression plus modifications specified by any cast type and
      /// byte offset, or return \c NULL if no expression is stored.
      ///
      /// The caller is responsible for the memory of the result.
      SgExpression *completeExpression() const;
      /// \brief Return the type of this expression using modifications
      /// specified by any cast type, or return \c NULL if no expression
      /// is stored.
      SgType *getCompleteType() const {
        ROSE2LLVM_ASSERT(attributes);
        if (expr) {
          return castType ? castType : expr->get_type();
        }
        return NULL;
      }
      /// \brief Return true iff an expression is stored and it is an
      /// \c SgAddressOfOp, is an \c SgStringVal, has type
      /// \c SgArrayType, or has type \c SgFunctionType.
      bool hasAddressBase() const {
        ROSE2LLVM_ASSERT(attributes);
        if (!expr) return false;
        SgType *expr_type =
          expr->get_type()->stripTypedefsAndModifiers();
        return
          isSgAddressOfOp(expr) || isSgStringVal(expr)
          || isSgArrayType(expr_type) || isSgFunctionType(expr_type);
      }
      /// \brief Return true iff <tt>hasAddressBase()</tt> is true and
      /// there is no byte offset.
      bool hasNonZeroAddressBase() const {
        return hasAddressBase() && !byteOffset;
      }
      /// \brief Return true iff an expression is stored and it is an
      /// \c SgValueExp of integer type.
      bool hasIntegerBase() const {
        ROSE2LLVM_ASSERT(attributes);
        if (!expr) return false;
        SgType *expr_type =
          expr->get_type()->stripTypedefsAndModifiers();
        return
          isSgValueExp(expr)
          && (expr_type->isIntegerType() || isSgEnumType(expr_type));
      }
      /// \brief Return true iff either <tt>hasAddressBase()</tt> or
      /// <tt>hasIntegerBase()</tt> is true.
      bool hasConstantBase() const {
        ROSE2LLVM_ASSERT(attributes);
        return hasAddressBase() || hasIntegerBase();
      }
      /// \brief Get the cast type or \c NULL if none.
      SgType *getCastType() const {
        ROSE2LLVM_ASSERT(attributes);
        return castType;
      }
      /// \brief Return true iff \c type is a pointer type or an integer
      /// type whose size is equal to or larger than a pointer.  The
      /// result is dependent upon \c this only for the associated
      /// \c LLVMAstAttributes.
      ///
      /// FIXME: The result can vary based on the target platform.  For
      /// example, what is the sizeof(long long)?  ROSE/EDG doesn't
      /// know the target platform, so it can't complain when a global
      /// initializer contains conversions of pointers to integers that
      /// are too small.  Thus, we can't just refuse to fold them.  We
      /// must somehow report an error to the user or a warning and then
      /// truncate and fold.
      bool canTypeStorePointer(SgType *type) {
        ROSE2LLVM_ASSERT(attributes);
        SgType *t = type->stripTypedefsAndModifiers();
        if (isSgPointerType(t)) {
          return true;
        }
        if (!t->isIntegerType() && !isSgEnumType(t)) {
          return false;
        }
        SgType *void_ptr =
          SageBuilder::buildPointerType(SageBuilder::buildVoidType());
        attributes->setLLVMTypeName(t);
        attributes->setLLVMTypeName(void_ptr);
        size_t t_size =
          ((IntAstAttribute*)t->getAttribute(Control::LLVM_SIZE))
            ->getValue();
        size_t void_ptr_size =
          ((IntAstAttribute*)void_ptr->getAttribute(Control::LLVM_SIZE))
            ->getValue();
        return void_ptr_size <= t_size;
      }
      /// \brief Set the cast type, overwriting any previous cast type.
      ///
      /// Invoke only if an expression is stored.  Invoke with
      /// non-<tt>NULL</tt> only if either (1) <tt>hasIntegerBase()</tt>
      /// is true while the new cast type is of pointer type or (2)
      /// <tt>hasAddressBase()</tt> is true while \c canTypeStorePointer
      /// returns true for the new cast type.  In all other cases,
      /// either (1) the cast can be performed immediately and its
      /// result stored in place of the stored expression or (2) the
      /// cast cannot appear in a constant expression, so it must be
      /// left unfolded.  FIXME: It never seems necessary to invoke
      /// with \c NULL.
      void setCastType(SgType *cast_type) {
        ROSE2LLVM_ASSERT(attributes);
        ROSE2LLVM_ASSERT(expr);
        ROSE2LLVM_ASSERT(
          !cast_type
          || (
            hasIntegerBase()
            && isSgPointerType(cast_type->stripTypedefsAndModifiers())
          )
          || (hasAddressBase() && canTypeStorePointer(cast_type))
        );
        castType = cast_type;
      }
      /// \brief Get the byte offset or zero if none.
      ///
      /// This is always a byte count not an element count.
      long long getByteOffset() const {
        ROSE2LLVM_ASSERT(attributes);
        return byteOffset;
      }
      /// \brief Set the byte offset type, overwriting any previous byte
      /// offset.
      ///
      /// This is always a byte count not an element count.  Invoke only
      /// if <tt>hasAddressBase()</tt> is true.  In all other cases,
      /// either (1) the arithmetic can be performed immediately and its
      /// results stored in place of the stored expression or (2) the
      /// arithmetic cannot appear in a constant expression, so it must
      /// be left unfolded.
      void setByteOffset(long long byte_offset) {
        ROSE2LLVM_ASSERT(attributes);
        ROSE2LLVM_ASSERT(hasAddressBase());
        byteOffset = byte_offset;
      }
      /// \brief Add a note that the parent node for the stored
      /// expression implies an address-of operation and that operation
      /// should be skipped as it has already been folded with its
      /// child.
      ///
      /// Invoke only if an expression is stored.
      void setOmitParentAddressOfOp() {
        ROSE2LLVM_ASSERT(attributes);
        ROSE2LLVM_ASSERT(expr);
        omitParentAddressOfOp = true;
      }
      /// \brief Iff there is a note that the address-of operation
      /// implied by the parent node for the stored expression should be
      /// skipped, return true and remove the note.
      ///
      /// Invoke only if an expression is stored.
      bool popOmitParentAddressOfOp() {
        ROSE2LLVM_ASSERT(attributes);
        bool result = omitParentAddressOfOp;
        omitParentAddressOfOp = false;
        return result;
      }
    private:
      void clear();
      SgExpression *expr;
      unsigned long *refCount;
      /// In general with ROSE, you should never modify an \c SgType
      /// because it is often shared.  However, ROSE functions like
      /// <tt>SageBuilder::buildCastExp</tt> take a non-const \c SgType,
      /// so \c SgType is only conceptually const.  Thus, \c castType
      /// must be mutable so \c getCastType can return non-const for a
      /// const \c SynthesizedAttribute.
      mutable SgType *castType;
      long long byteOffset;
      bool omitParentAddressOfOp;
      LLVMAstAttributes *attributes;
  };
  /**
   * \brief
   *   Folds an individual constant expression and returns the resulting
   *   expression without modifying the tree.
   *
   * \c originalExpressionTree properties in the tree are ignored.
   */
  class ExpressionTraversal
    : protected AstTopDownBottomUpProcessing<
        InheritedAttribute, SynthesizedAttribute
      >
  {
    public:
      /// The caller is responsible for the memory of the result.
      SgExpression *fold(SgExpression *expr) {
        SgStringList noArgs;
        Option option(noArgs);
        Control control(option);
        attributes = new LLVMAstAttributes(
          option, control, expr->getFilenameString()
        );
        control.SetAttribute(
          expr, Control::LLVM_AST_ATTRIBUTES, attributes
        );
        return AstTopDownBottomUpProcessing<
          InheritedAttribute, SynthesizedAttribute
        >::traverse(expr, InheritedAttribute()).completeExpression();
      }
    private:
      InheritedAttribute evaluateInheritedAttribute(
        SgNode *node, InheritedAttribute inherited_attribute
      );
      SynthesizedAttribute evaluateSynthesizedAttribute(
        SgNode *node, InheritedAttribute inherited_attribute,
        SynthesizedAttributesList synthesized_attribute_list
      );
      SynthesizedAttribute foldArithmeticUnaryOp(
        SgUnaryOp const *op, SynthesizedAttribute operand_attr
      );
      SynthesizedAttribute foldCast(
        SgType *result_type, SynthesizedAttribute operand_attr,
        bool scale_for_pointer = false
      );
      SynthesizedAttribute foldPointerDerefExp(
        SynthesizedAttribute operand_attr, SgNode const *parent
      );
      SynthesizedAttribute foldUnaryOp(
        SgUnaryOp const *op, SynthesizedAttribute operand_attr
      );
      SynthesizedAttribute foldArithmeticBinaryOp(
        SgBinaryOp const *op, SynthesizedAttribute lhs_attr,
        SynthesizedAttribute rhs_attr, bool scale_for_pointer = true
      );
      SynthesizedAttribute foldArrowExp(
        SgBinaryOp const *op,
        SynthesizedAttribute lhs_attr, SynthesizedAttribute rhs_attr
      );
      SynthesizedAttribute foldBinaryOp(
        SgBinaryOp const *op,
        SynthesizedAttribute lhs_attr, SynthesizedAttribute rhs_attr
      );
      SynthesizedAttribute foldConditionalExp(
        SynthesizedAttribute c_attr, SynthesizedAttribute t_attr,
        SynthesizedAttribute f_attr
      );
      LLVMAstAttributes *attributes;
      // Unimplemented.
      void traverse(SgNode *node);
      void traverseWithinFile(SgNode *node);
      void traverseInputFiles(SgProject *node);
  };
}

#endif
