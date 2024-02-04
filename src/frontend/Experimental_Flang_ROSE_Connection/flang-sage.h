#pragma once

// Forward reference
class SgExpression;

namespace Fortran::parser {

//erasmus WalkE no no
#if 0
// Trait-determined traversal of empty, tuple, union, wrapper,
// and constraint-checking classes.
template <typename A, typename V>
std::enable_if_t<EmptyTrait<A>> WalkE(const A &x, SgExpression* &sg, V &visitor) {
  if (visitor.Pre(x, sg)) {
    visitor.Post(x, sg);
  }
}
template <typename A, typename M>
std::enable_if_t<EmptyTrait<A>> WalkE(A &x, SgExpression* &sg, M &mutator) {
  if (mutator.Pre(x, sg)) {
    mutator.Post(x, sg);
  }
}

template <typename A, typename V>
std::enable_if_t<TupleTrait<A>> WalkE(const A &x, SgExpression* &sg, V &visitor) {
  if (visitor.Pre(x, sg)) {
    WalkE(x.t, sg, visitor);
    visitor.Post(x, sg);
  }
}
template <typename A, typename M>
std::enable_if_t<TupleTrait<A>> WalkE(A &x, SgExpression* &sg, M &mutator) {
  if (mutator.Pre(x, sg)) {
    WalkE(x.t, sg, mutator);
    mutator.Post(x, sg);
  }
}

#if 0
template <typename A, typename V>
std::enable_if_t<UnionTrait<A>> Walk(const A &x, V &visitor, SgExpression* &sg) {
  if (visitor.Pre(x, sg)) {
    Walk(x.u, visitor, sg);
    visitor.Post(x, sg);
  }
}
#endif
template <typename A, typename M>
std::enable_if_t<UnionTrait<A>> WalkE(A &x, SgExpression* &sg, M &mutator) {
  if (mutator.Pre(x, sg)) {
    WalkE(x.u, sg, mutator);
    mutator.Post(x, sg);
  }
}

template <typename A, typename V>
std::enable_if_t<WrapperTrait<A>> WalkE(const A &x, SgExpression* &sg, V &visitor) {
  if (visitor.Pre(x, sg)) {
    WalkE(x.v, sg, visitor);
    visitor.Post(x, sg);
  }
}
template <typename A, typename M>
std::enable_if_t<WrapperTrait<A>> WalkE(A &x, SgExpression* &sg, M &mutator) {
  if (mutator.Pre(x, sg)) {
    WalkE(x.v, sg, mutator);
    mutator.Post(x, sg);
  }
}

template <typename A, typename V>
std::enable_if_t<ConstraintTrait<A>> WalkE(const A &x, SgExpression* &sg, V &visitor) {
  if (visitor.Pre(x, sg)) {
    WalkE(x.thing, sg, visitor);
    visitor.Post(x, sg);
  }
}
template <typename A, typename M>
std::enable_if_t<ConstraintTrait<A>> WalkE(A &x, SgExpression* &sg, M &mutator) {
  if (mutator.Pre(x, sg)) {
    WalkE(x.thing, sg, mutator);
    mutator.Post(x, sg);
  }
}

template <typename T, typename V>
void WalkE(const common::Indirection<T> &x, SgExpression* &sg, V &visitor) {
  WalkE(x.value(), sg, visitor);
}
template <typename T, typename M>
void WalkE(common::Indirection<T> &x, SgExpression* &sg, M &mutator) {
  WalkE(x.value(), sg, mutator);
}
#endif

//--------- end of walk with sage expr -------------

class FlangExprVisitor {
public:
  FlangExprVisitor() {}

  // In nearly all cases, this code avoids defining Boolean-valued Pre()
  // callbacks for the parse tree walking framework in favor of two void
  // functions, Before() and Build(), which imply true and false return
  // values for Pre() respectively.
  template <typename T> void Before(const T &, SgExpression* &) {}
//erasmus: HUM
#if 0
  //original
  template <typename T> double Build(const T &); // not void, never used
#else
  template <typename T> double Build(const T &x) {info(x,"double crap ");} // not void, never used (but seems to)
#endif

  template <typename T> bool Pre(const T &x, SgExpression* &sg) {
    if constexpr (std::is_void_v<decltype(Build(x))>) {
      // There is a local definition of Build() for this type.  It
      // overrides the parse tree walker's default Walk() over the descendents.
      Before(x, sg);
      //erasmus: Hopefully can get this far someday
      // Build(x, sg);
      abort();
      Post(x, sg);
      return false; // Walk() does not visit descendents
    }
    else {
      Before(x, sg);
      return true; // there's no Build() defined here, Walk() the descendents
    }
  }

  template <typename T> void Post(const T &, SgExpression* &) {}

  // IntrinsicOperator
  void Build(const parser::Expr::Power &);
  void Build(const parser::Expr::Multiply &);
  void Build(const parser::Expr::Divide &);
  void Build(const parser::Expr::Add &);
  void Build(const parser::Expr::Subtract &);
  void Build(const parser::Expr::Concat &);
  void Build(const parser::Expr::LT &);
  void Build(const parser::Expr::LE &);
  void Build(const parser::Expr::EQ &);
  void Build(const parser::Expr::NE &);
  void Build(const parser::Expr::GE &);
  void Build(const parser::Expr::GT &);
  void Build(const parser::Expr::NOT &);
  void Build(const parser::Expr::AND &);
  void Build(const parser::Expr::OR &);
  void Build(const parser::Expr::EQV &);
  void Build(const parser::Expr::NEQV &);

  void Build(const parser::Name &);
  void Build(const parser::IntLiteralConstant &);

  void Done(SgExpression* &) const {}

  // Call back to the expression traversal framework.
  template <typename T> void Walk(const T &x, SgExpression* &sg) {
    Walk(x, *this, sg);
  }

}; // FlangExprVisitor

//erasmus: NOTE: IntLiteralConstant not in parse-tree-visitor.h? Probably because it is a terminal!
template <typename V> void Walk(const IntLiteralConstant &x, V &visitor, SgExpression* &sg) {
#if 1
  if (visitor.Pre(x, sg)) {
    std::cerr << "...I am a terminal...\n";
    //    Build(x, sg);
    abort();
    //    Walk(x.source, visitor);
    //    Walk(x.t, visitor);
    visitor.Post(x, sg);
  }
#endif
}
template <typename M> void Walk(IntLiteralConstant &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.source, mutator);
    Walk(x.t, mutator);
    mutator.Post(x);
  }
#endif
}

//erasmus
//Working on specializations
void WalkWith(parser::IntLiteralConstant &x, SgExpression* &sg) {
  FlangExprVisitor visitor{};
  Walk(x, visitor, sg);
  visitor.Done(sg);
}

//erasmus
//Working on
template <typename T>
void WalkWith(T &x, SgExpression* &sg) {
  FlangExprVisitor visitor{};
#if 1
  Walk(x, visitor, sg);
#endif
  visitor.Done(sg);
}

//------ from parse-tree-visitor -------

// Trait-determined traversal of empty, tuple, union, wrapper,
// and constraint-checking classes.
template <typename A, typename V>
std::enable_if_t<EmptyTrait<A>> Walk(const A &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    visitor.Post(x);
  }
#endif
}
template <typename A, typename M>
std::enable_if_t<EmptyTrait<A>> Walk(A &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    mutator.Post(x);
  }
#endif
}

template <typename A, typename V>
std::enable_if_t<TupleTrait<A>> Walk(const A &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.t, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename A, typename M>
std::enable_if_t<TupleTrait<A>> Walk(A &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.t, mutator);
    mutator.Post(x);
  }
#endif
}
 
template <typename A, typename V>
std::enable_if_t<UnionTrait<A>> Walk(const A &x, V &visitor, SgExpression* &sg) {
#if 1
//erasmus: WORKING ON
  if (visitor.Pre(x, sg)) {
    Walk(x.u, visitor, sg);
    visitor.Post(x, sg);
  }
#endif
}
template <typename A, typename M>
std::enable_if_t<UnionTrait<A>> Walk(A &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x, sg)) {
    Walk(x.u, mutator, sg);
    mutator.Post(x, sg);
  }
#endif
}

template <typename A, typename V>
std::enable_if_t<WrapperTrait<A>> Walk(const A &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x, sg)) {
    Walk(x.v, visitor, sg);
    visitor.Post(x, sg);
  }
#endif
}
template <typename A, typename M>
std::enable_if_t<WrapperTrait<A>> Walk(A &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x, sg)) {
    Walk(x.v, mutator, sg);
    mutator.Post(x, sg);
  }
#endif
}

template <typename A, typename V>
std::enable_if_t<ConstraintTrait<A>> Walk(const A &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.thing, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename A, typename M>
std::enable_if_t<ConstraintTrait<A>> Walk(A &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.thing, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename T, typename V>
void Walk(const common::Indirection<T> &x, V &visitor, SgExpression* &sg) {
#if 0
  WalkE(x.value(), sg, visitor);
#endif
}
template <typename T, typename M>
void WalkE(common::Indirection<T> &x, M &mutator, SgExpression* &sg) {
#if 0
  WalkE(x.value(), sg, mutator);
#endif
}

template <typename T, typename V> void Walk(const Statement<T> &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    // N.B. The label, if any, is not visited.
    Walk(x.source, visitor);
    Walk(x.statement, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename T, typename M> void Walk(Statement<T> &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    // N.B. The label, if any, is not visited.
    Walk(x.source, mutator);
    Walk(x.statement, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename T, typename V>
void Walk(const UnlabeledStatement<T> &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x, sg)) {
    Walk(x.source, visitor, sg);
    Walk(x.statement, visitor, sg);
    visitor.Post(x, sg);
  }
#endif
}
template <typename T, typename M>
void Walk(UnlabeledStatement<T> &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x, sg)) {
    Walk(x.source, mutator, sg);
    Walk(x.statement, mutator, sg);
    mutator.Post(x, sg);
  }
#endif
}

template <typename V> void Walk(const Name &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.source, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(Name &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.source, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const AcSpec &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.type, visitor);
    Walk(x.values, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(AcSpec &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.type, mutator);
    Walk(x.values, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const ArrayElement &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.base, visitor);
    Walk(x.subscripts, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(ArrayElement &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.base, mutator);
    Walk(x.subscripts, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const CharSelector::LengthAndKind &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.length, visitor);
    Walk(x.kind, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(CharSelector::LengthAndKind &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.length, mutator);
    Walk(x.kind, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const CaseValueRange::Range &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.lower, visitor);
    Walk(x.upper, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(CaseValueRange::Range &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.lower, mutator);
    Walk(x.upper, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const CoindexedNamedObject &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.base, visitor);
    Walk(x.imageSelector, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(CoindexedNamedObject &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.base, mutator);
    Walk(x.imageSelector, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const DeclarationTypeSpec::Class &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.derived, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(DeclarationTypeSpec::Class &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.derived, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const DeclarationTypeSpec::Type &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.derived, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(DeclarationTypeSpec::Type &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.derived, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const ImportStmt &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.names, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(ImportStmt &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.names, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const IntrinsicTypeSpec::Character &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.selector, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(IntrinsicTypeSpec::Character &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.selector, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const IntrinsicTypeSpec::Complex &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.kind, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(IntrinsicTypeSpec::Complex &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.kind, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const IntrinsicTypeSpec::Logical &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.kind, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(IntrinsicTypeSpec::Logical &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.kind, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const IntrinsicTypeSpec::Real &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.kind, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(IntrinsicTypeSpec::Real &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.kind, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename A, typename B, typename V>
void Walk(const LoopBounds<A, B> &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.name, visitor);
    Walk(x.lower, visitor);
    Walk(x.upper, visitor);
    Walk(x.step, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename A, typename B, typename M>
void Walk(LoopBounds<A, B> &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.name, mutator);
    Walk(x.lower, mutator);
    Walk(x.upper, mutator);
    Walk(x.step, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const CommonStmt &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.blocks, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(CommonStmt &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.blocks, mutator);
    mutator.Post(x);
  }
#endif
}


//---------------------- add below ------------

template <typename V> void Walk(const Designator &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x, sg)) {
    Walk(x.source, visitor);
    Walk(x.u, visitor);
    visitor.Post(x, sg);
  }
#endif
}
template <typename M> void Walk(Designator &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x, sg)) {
    Walk(x.source, mutator);
    Walk(x.u, mutator);
    mutator.Post(x, sg);
  }
#endif
}
template <typename V> void Walk(const FunctionReference &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.source, visitor);
    Walk(x.v, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(FunctionReference &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.source, mutator);
    Walk(x.v, mutator);
    mutator.Post(x);
  }
#endif
}

//---------------------- add above ------------

//---------------------- add above ------------

template <typename V> void Walk(const SignedIntLiteralConstant &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.source, visitor);
    Walk(x.t, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(SignedIntLiteralConstant &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.source, mutator);
    Walk(x.t, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const RealLiteralConstant &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.real, visitor);
    Walk(x.kind, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(RealLiteralConstant &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.real, mutator);
    Walk(x.kind, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
 void Walk(const RealLiteralConstant::Real &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.source, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(RealLiteralConstant::Real &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.source, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const StructureComponent &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.base, visitor);
    Walk(x.component, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(StructureComponent &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.base, mutator);
    Walk(x.component, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const Suffix &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.binding, visitor);
    Walk(x.resultName, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(Suffix &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.binding, mutator);
    Walk(x.resultName, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
 void Walk(const TypeBoundProcedureStmt::WithInterface &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.interfaceName, visitor);
    Walk(x.attributes, visitor);
    Walk(x.bindingNames, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
 void Walk(TypeBoundProcedureStmt::WithInterface &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.interfaceName, mutator);
    Walk(x.attributes, mutator);
    Walk(x.bindingNames, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
 void Walk(const TypeBoundProcedureStmt::WithoutInterface &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.attributes, visitor);
    Walk(x.declarations, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
 void Walk(TypeBoundProcedureStmt::WithoutInterface &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.attributes, mutator);
    Walk(x.declarations, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const UseStmt &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.nature, visitor);
    Walk(x.moduleName, visitor);
    Walk(x.u, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(UseStmt &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.nature, mutator);
    Walk(x.moduleName, mutator);
    Walk(x.u, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const WriteStmt &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.iounit, visitor);
    Walk(x.format, visitor);
    Walk(x.controls, visitor);
    Walk(x.items, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(WriteStmt &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.iounit, mutator);
    Walk(x.format, mutator);
    Walk(x.controls, mutator);
    Walk(x.items, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const format::ControlEditDesc &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.kind, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(format::ControlEditDesc &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.kind, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
 void Walk(const format::DerivedTypeDataEditDesc &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.type, visitor);
    Walk(x.parameters, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(format::DerivedTypeDataEditDesc &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.type, mutator);
    Walk(x.parameters, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V> void Walk(const format::FormatItem &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.repeatCount, visitor);
    Walk(x.u, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(format::FormatItem &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.repeatCount, mutator);
    Walk(x.u, mutator);
    mutator.Post(x);
  }
#endif
}
 
template <typename V>
void Walk(const format::FormatSpecification &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.items, visitor);
    Walk(x.unlimitedItems, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(format::FormatSpecification &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.items, mutator);
    Walk(x.unlimitedItems, mutator);
    mutator.Post(x);
  }
#endif
}
 
template <typename V>
void Walk(const format::IntrinsicTypeDataEditDesc &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.kind, visitor);
    Walk(x.width, visitor);
    Walk(x.digits, visitor);
    Walk(x.exponentWidth, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(format::IntrinsicTypeDataEditDesc &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.kind, mutator);
    Walk(x.width, mutator);
    Walk(x.digits, mutator);
    Walk(x.exponentWidth, mutator);
    mutator.Post(x);
  }
#endif
}
template <typename V> void Walk(const CompilerDirective &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.source, visitor);
    Walk(x.u, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(CompilerDirective &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.source, mutator);
    Walk(x.u, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const OmpLinearClause::WithModifier &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.modifier, visitor);
    Walk(x.names, visitor);
    Walk(x.step, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M> void Walk(OmpLinearClause::WithModifier &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.modifier, mutator);
    Walk(x.names, mutator);
    Walk(x.step, mutator);
    mutator.Post(x);
  }
#endif
}

template <typename V>
void Walk(const OmpLinearClause::WithoutModifier &x, V &visitor, SgExpression* &sg) {
#if 0
  if (visitor.Pre(x)) {
    Walk(x.names, visitor);
    Walk(x.step, visitor);
    visitor.Post(x);
  }
#endif
}
template <typename M>
void Walk(OmpLinearClause::WithoutModifier &x, M &mutator, SgExpression* &sg) {
#if 0
  if (mutator.Pre(x)) {
    Walk(x.names, mutator);
    Walk(x.step, mutator);
    mutator.Post(x);
  }
#endif
}

//---------------------- finished? ------------


//erasmus: I added this?
#if 0
void WalkE(const parser::Variable &x, SgExpression* &sg, FlangExprVisitor &visitor) {
  std::cerr << "\n\n!!!!!!!!!!!!!!! HELP ---------------\n\n";
  //  abort();
  //WalkE(x.u, visitor, sg);
  Walk(x, visitor, sg);
}

template <typename T>
void WalkE(const T &x, SgExpression* &sg, FlangExprVisitor &visitor) {
  std::cerr << "\n\n!!!!!!!!!!!!!!! HELP ---------------\n\n";
  abort();
  //  WalkE(x.value(), sg, visitor);
}
#endif

} // namespace Rose::builder
