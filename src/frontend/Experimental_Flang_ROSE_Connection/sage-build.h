#ifndef ROSE_BUILDER_BUILD_H_
#define ROSE_BUILDER_BUILD_H_

// HACK-TMP-TODO
#define FLANG_LITTLE_ENDIAN 1

// Using this macro to make unneeded build (not in visitor) functions go away
#define USE_DEPRECATED 0

// These includes are from the F18/flang source tree (./lib/parser)
//
#include "flang/Parser/parse-tree.h"
#include "flang/Parser/parse-tree-visitor.h"
#include "flang/Semantics/symbol.h"
#include "flang/Semantics/scope.h"
#include <iostream>
#include <typeinfo>
#include <boost/optional.hpp>

// Testing building everything from flang unparser
#include "unparse-sage.h"

#include "../Experimental_General_Language_Support/general_language_translation.h"

using OptLabel = std::optional<unsigned long long>;
using EntityDeclTuple = std::tuple<std::string, SgType*, SgExpression*>;

// Needed until Rose compiles with C++17 (see setSgSourceFile below)
class SgSourceFile;

namespace Rose::builder {

// SgSourceFile* temporary needed until ROSE supports C++17
void setSgSourceFile(SgSourceFile* sg_file);

// Converts parsed program to ROSE Sage nodes
void Build(Fortran::parser:: Program &, Fortran::parser::AllCookedSources &);

void Build(Fortran::parser:: CompilerDirective &);
void Build(Fortran::parser:: OpenACCRoutineConstruct &);
void Build(Fortran::parser:: OpenMPDeclarativeConstruct &);
void Build(Fortran::parser:: OpenACCDeclarativeConstruct &);

void Build(Fortran::parser:: OpenMPConstruct &);
void Build(Fortran::parser:: OpenACCConstruct &);
void Build(Fortran::parser:: AccEndCombinedDirective &);
void Build(Fortran::parser:: OmpEndLoopDirective &);
void Build(Fortran::parser:: CUFKernelDoConstruct &);

void BuildFunctionReturnType(const Fortran::parser::SpecificationPart &, std::string &, SgType* &);

void BuildImpl(Fortran::parser::AssignmentStmt &);

void Build(Fortran::parser::FunctionStmt &, std::list<std::string> &, std::string &, std::string &, LanguageTranslation::FunctionModifierList &, SgType* &);
void Build(Fortran::parser::SubroutineStmt &, std::list<std::string> &, std::string &, LanguageTranslation::FunctionModifierList &);

void Build(Fortran::parser::             Substring &, SgExpression* &);
void Build(Fortran::parser::     FunctionReference &, SgExpression* &);
void Build(Fortran::parser::                  Call &, std::list<SgExpression*> &arg_list, std::string &name);
void Build(Fortran::parser::      ProcComponentRef &, SgExpression* &);

void Build(Fortran::parser::ActualArgSpec &, SgExpression* &);

void Build(Fortran::parser::               Keyword &, SgExpression* &);
void Build(Fortran::parser::         NamedConstant &, SgExpression* &);
void Build(Fortran::parser:: Expr::IntrinsicBinary &, SgExpression* &);

// LiteralConstant
void BuildImpl(Fortran::parser::HollerithLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::IntLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::SignedIntLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::RealLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::SignedRealLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::ComplexLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::BOZLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::CharLiteralConstant &, SgExpression* &);
void BuildImpl(Fortran::parser::LogicalLiteralConstant &, SgExpression* &);

void BuildImpl(Fortran::parser::KindSelector::StarSize &, SgExpression* &);
void BuildImpl(Fortran::parser::CharLength &, SgExpression* &);
void BuildImpl(Fortran::parser::TypeParamValue &, SgExpression* &);

// CommonBlockObject
void BuildImpl(Fortran::parser::CommonBlockObject &, SgExpression* &);

// ArraySpec ...
void BuildImpl(Fortran::parser::AssumedImpliedSpec &, SgExpression* &);
void BuildImpl(Fortran::parser::ExplicitShapeSpec &, SgExpression* &);
void BuildImpl(Fortran::parser::AssumedShapeSpec &, SgExpression* &);

// KindParam
void BuildImpl(std::optional<Fortran::parser::KindParam> &, std::uint64_t &, std::string &);

// InternalSubprogramPart
void Build(Fortran::parser::InternalSubprogramPart &);

// ImplicitPart
void BuildImpl(Fortran::parser:: ParameterStmt &);
void BuildImpl(Fortran::parser:: OldParameterStmt &);
void BuildImpl(Fortran::parser:: FormatStmt &);
void BuildImpl(Fortran::parser:: EntryStmt &);

void BuildImpl(Fortran::parser::UseStmt &);

void Build(std::list<Fortran::parser::ImplicitSpec> &, std::list<std::tuple<SgType*, std::list<std::tuple<char, boost::optional<char>>>>> &implicit_spec_list);
void Build(Fortran::parser::ImplicitSpec &, SgType* &type, std::list<std::tuple<char, boost::optional<char>>> &letter_spec_list);
void Build(std::list<Fortran::parser::LetterSpec> &, std::list<std::tuple<char, boost::optional<char>>> &letter_spec_list);
void Build(Fortran::parser::LetterSpec &, std::tuple<char, boost::optional<char>> &letter_spec);

void Build(Fortran::parser::  DeclarationConstruct &);
void Build(Fortran::parser::SpecificationConstruct &);

void Build(Fortran::parser::   DeclarationTypeSpec &, SgType* &);

// DeclarationTypeSpec
void Build(Fortran::parser::DeclarationTypeSpec::     Type &, SgType* &);
void Build(Fortran::parser::DeclarationTypeSpec:: TypeStar &, SgType* &);
void Build(Fortran::parser::DeclarationTypeSpec::    Class &, SgType* &);
void Build(Fortran::parser::DeclarationTypeSpec::ClassStar &, SgType* &);

void Build(Fortran::parser::        VectorTypeSpec &, SgType* &);
void Build(Fortran::parser::       DerivedTypeSpec &, SgType* &);

void EntityDecls(std::list<Fortran::parser::EntityDecl> &, std::list<EntityDeclTuple> &, SgType*);

void Build(Fortran::parser::              AttrSpec &, LanguageTranslation::ExpressionKind &modifier_enum);
void Build(Fortran::parser::             ArraySpec &, SgType* &, SgType*);
void Build(Fortran::parser::           CoarraySpec &, SgType* &, SgType*);
void Build(Fortran::parser::       IntegerTypeSpec &, SgType* &);
#if USE_DEPRECATED
void Build(Fortran::parser::            CharLength &, SgExpression* &);
#endif
void Build(Fortran::parser::        Initialization &, SgExpression* &);

void Build(Fortran::parser::                   SpecificationExpr &, SgExpression* &);
void Build(Fortran::parser::Scalar<Fortran::parser::IntExpr>     &, SgExpression* &);
void Build(Fortran::parser::Scalar<Fortran::parser::LogicalExpr> &, SgExpression* &);
void Build(Fortran::parser::                        ConstantExpr &, SgExpression* &);

void Build(Fortran::parser::IntrinsicTypeSpec:: Real &);
void Build(Fortran::parser::IntrinsicTypeSpec:: DoublePrecision &);
void Build(Fortran::parser::IntrinsicTypeSpec:: Complex &);
void Build(Fortran::parser::IntrinsicTypeSpec:: Character &);
void Build(Fortran::parser::IntrinsicTypeSpec:: Logical &);
void Build(Fortran::parser::IntrinsicTypeSpec:: DoubleComplex &);

#if USE_DEPRECATED
void Build(Fortran::parser::               CharSelector &, SgExpression* &);
void Build(Fortran::parser::             LengthSelector &, SgExpression* &);
void Build(Fortran::parser::CharSelector::LengthAndKind &, SgExpression* &);
void Build(Fortran::parser::             TypeParamValue &, SgExpression* &);
#endif

// DeclarationConstruct
void Build(Fortran::parser::StmtFunctionStmt &);
void Build(Fortran::parser::   ErrorRecovery &);

// ActionStmt
void Build(Fortran::parser::         ContinueStmt &, const OptLabel &);
void Build(Fortran::parser::            CycleStmt &, const OptLabel &);
void Build(Fortran::parser::        FailImageStmt &, const OptLabel &);
void Build(Fortran::parser::             StopStmt &, const OptLabel &);

void Build(Fortran::parser::         ContinueStmt &);
void Build(Fortran::parser::        FailImageStmt &);

void Build(Fortran::parser::         AllocateStmt &);
void Build(Fortran::parser::        BackspaceStmt &);
void Build(Fortran::parser::             CallStmt &);
void Build(Fortran::parser::            CloseStmt &);
void Build(Fortran::parser::            CycleStmt &);
void Build(Fortran::parser::       DeallocateStmt &);
void Build(Fortran::parser::          EndfileStmt &);
void Build(Fortran::parser::        EventPostStmt &);
void Build(Fortran::parser::        EventWaitStmt &);
void Build(Fortran::parser::             ExitStmt &);
void Build(Fortran::parser::            FlushStmt &);
void Build(Fortran::parser::         FormTeamStmt &);
void Build(Fortran::parser::             GotoStmt &);
void Build(Fortran::parser::               IfStmt &);
void Build(Fortran::parser::          InquireStmt &);
void Build(Fortran::parser::             LockStmt &);
void Build(Fortran::parser::          NullifyStmt &);
void Build(Fortran::parser::             OpenStmt &);
void Build(Fortran::parser::PointerAssignmentStmt &);
void Build(Fortran::parser::            PrintStmt &);

void Build(Fortran::parser::      DefaultCharExpr &, SgExpression* &);
void Build(Fortran::parser::                Label &, SgExpression* &);
void Build(Fortran::parser::                 Star &, SgExpression* &);
void Build(Fortran::parser::           OutputItem &, SgExpression* &);
void Build(Fortran::parser::      OutputImpliedDo &);

void Build(Fortran::parser::             ReadStmt &);
void Build(Fortran::parser::           ReturnStmt &);
void Build(Fortran::parser::           RewindStmt &);
void Build(Fortran::parser::          SyncAllStmt &);
void Build(Fortran::parser::       SyncImagesStmt &);
void Build(Fortran::parser::       SyncMemoryStmt &);
void Build(Fortran::parser::         SyncTeamStmt &);
void Build(Fortran::parser::           UnlockStmt &);
void Build(Fortran::parser::             WaitStmt &);
void Build(Fortran::parser::            WhereStmt &);
void Build(Fortran::parser::            WriteStmt &);
void Build(Fortran::parser::     ComputedGotoStmt &);
void Build(Fortran::parser::           ForallStmt &);
void Build(Fortran::parser::     ArithmeticIfStmt &);
void Build(Fortran::parser::           AssignStmt &);
void Build(Fortran::parser::     AssignedGotoStmt &);
void Build(Fortran::parser::            PauseStmt &);
void Build(Fortran::parser::         NamelistStmt &);

// Expr
//
void Build(Fortran::parser::CharLiteralConstantSubstring &, SgExpression* &);
void Build(Fortran::parser::SubstringInquiry &, SgExpression* &);

void Build(Fortran::parser::         ArrayConstructor &, SgExpression* &);
void Build(Fortran::parser::                   AcSpec &, SgExpression* &);
void Build(Fortran::parser::     StructureConstructor &, SgExpression* &);
void Build(Fortran::parser::      Expr::DefinedBinary &, SgExpression* &);
void Build(Fortran::parser:: Expr::ComplexConstructor &, SgExpression* &);
void Build(Fortran::parser::        Expr::Parentheses &, SgExpression* &);
void Build(Fortran::parser::          Expr::UnaryPlus &, SgExpression* &);
void Build(Fortran::parser::             Expr::Negate &, SgExpression* &);
void Build(Fortran::parser::                Expr::NOT &, SgExpression* &);

void Build(Fortran::parser::  StructureComponent &, SgExpression* &);
void Build(Fortran::parser::        ArrayElement &, SgExpression* &);
void Build(Fortran::parser::CoindexedNamedObject &, SgExpression* &);
void Build(Fortran::parser::       ImageSelector &, SgExpression* &);
void Build(Fortran::parser::   ImageSelectorSpec &, SgExpression* &);
void Build(Fortran::parser::    SectionSubscript &, SgExpression* &);
void Build(Fortran::parser::    SubscriptTriplet &, SgExpression* &);

// ExecutableConstruct
void Build(Fortran::parser::   AssociateConstruct&);
void Build(Fortran::parser::       BlockConstruct&);
void Build(Fortran::parser::        CaseConstruct&);
void Build(Fortran::parser::  CaseConstruct::Case&, SgStatement* &stmt);
void Build(Fortran::parser::             CaseStmt&, std::list<SgExpression*> &case_list);
void Build(Fortran::parser::CaseValueRange::Range&, SgExpression* &range);
void Build(Fortran::parser::    ChangeTeamConstruct&);
void Build(Fortran::parser::      CriticalConstruct&);
void Build(Fortran::parser::            LabelDoStmt&);
void Build(Fortran::parser::              EndDoStmt&);
void Build(Fortran::parser::            DoConstruct&);
void Build(Fortran::parser::            IfConstruct&);
void Build(Fortran::parser::    SelectRankConstruct&);
void Build(Fortran::parser::    SelectTypeConstruct&);
void Build(Fortran::parser::         WhereConstruct&);
void Build(Fortran::parser::        ForallConstruct&);

// DoConstruct
void Build(Fortran::parser::NonLabelDoStmt&, SgExpression* &name, SgExpression* &control);
void Build(Fortran::parser::LoopControl::Concurrent&, SgExpression* &);

// IfConstruct
void Build(Fortran::parser::              IfThenStmt&, SgExpression* &);
void Build(Fortran::parser::IfConstruct::  ElseBlock&, SgBasicBlock* &false_body);
void Build(std::list<Fortran::parser::IfConstruct::ElseIfBlock> &, SgBasicBlock* &else_if_block, SgIfStmt* &else_if_stmt);

// SpecificationConstruct
void Build(Fortran::parser::                    EnumDef&);
void Build(Fortran::parser::             InterfaceBlock&);
void Build(Fortran::parser::               StructureDef&);
void Build(Fortran::parser::     OtherSpecificationStmt&);
void Build(Fortran::parser::                GenericStmt&);
void Build(Fortran::parser::   ProcedureDeclarationStmt&);

void getSubroutineStmt(Fortran::parser::SubroutineStmt &, std::list<std::string> &,
                       LanguageTranslation::FunctionModifierList &);

// AttrSpec
void getAttrSpec(Fortran::parser:: AttrSpec &, std::list<LanguageTranslation::ExpressionKind> &, SgType* &);

void getModifiers(Fortran::parser:: AccessSpec &, LanguageTranslation::ExpressionKind &);
void getModifiers(Fortran::parser:: IntentSpec &, LanguageTranslation::ExpressionKind &);
void getModifiers(Fortran::parser:: LanguageBindingSpec &, LanguageTranslation::ExpressionKind &);
void getModifiers(Fortran::parser:: TypeAttrSpec &, LanguageTranslation::ExpressionKind &);

} // namespace Rose::builder

#endif // ROSE_BUILDER_BUILD_H_
