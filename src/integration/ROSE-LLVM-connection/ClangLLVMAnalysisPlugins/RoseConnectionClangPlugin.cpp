//===- RoseConnectionClangPlugin.cpp ---------------------------------------------===//
//
// Example clang plugin which incooperate ROSE. 
//
//===----------------------------------------------------------------------===//
#include "RoseConnectionClangPlugin.hpp"

using namespace clang;
namespace {

class collectInfoVisitor : public RecursiveASTVisitor<collectInfoVisitor> {

public:
  const char *LastLocFilename = "";
  unsigned LastLocLine = ~0U;
  explicit collectInfoVisitor(ASTContext *Context)
    : Context(Context) {}

  explicit collectInfoVisitor(ASTContext *Context,  llvm::StringRef inFile)
    : Context(Context), srcFile(inFile), SM(&(Context->getSourceManager())) {
//    llvm::outs() << "Constructing in " << srcFile << "\n";
  }

  void dumpPointer(const void *Ptr) {
     llvm::outs() << ' ' << Ptr;
  }


  void dumpLocation(std::string prefix,bool isbegin, SourceLocation Loc) {
    if (!SM)
      return;

    std::string jsonval = prefix;
    if(isbegin)
       jsonval += ".begin";
    else
       jsonval += ".end";

    SourceLocation SpellingLoc = SM->getSpellingLoc(Loc);

    // The general format we print out is filename:line:col, but we drop pieces
    // that haven't changed since the last loc printed.
    PresumedLoc PLoc = SM->getPresumedLoc(SpellingLoc);

    if (PLoc.isInvalid()) {
      llvm::outs() << "<invalid sloc>";
      return;
    }

    if (srcFile.compare(PLoc.getFilename()) == 0) {
      llvm::outs() << PLoc.getFilename() << ':' << PLoc.getLine() << ':' << PLoc.getColumn();
      LastLocFilename = PLoc.getFilename();
      LastLocLine = PLoc.getLine();
       
      pt.put(jsonval+"Line",PLoc.getLine());
      pt.put(jsonval+"Column",PLoc.getColumn());
    }
  }

  unsigned getLineNumber(SourceLocation Loc) {
    if (Loc.isInvalid())
      return 0;
    return SM->getPresumedLoc(Loc).getLine();
  }

  unsigned getColumnNumber(SourceLocation Loc, bool Force) {
    // If the location is invalid then use the current column.
    if (Loc.isInvalid()) 
      return 0;
    PresumedLoc PLoc = SM->getPresumedLoc(Loc);
    return PLoc.isValid() ? PLoc.getColumn() : 0;
  }

  void dumpSourceSingleLoc(std::string prefix, SourceLocation loc) {
      // Can't translate locations if a SourceManager isn't available.
    if (!SM)
      return;
    
    llvm::outs() << " <";
    dumpLocation(prefix,true, loc);
    llvm::outs() << ">";
  }

  void dumpSourceRange(std::string prefix, SourceRange R) {
    // Can't translate locations if a SourceManager isn't available.
    if (!SM)
      return;
    
    llvm::outs() << " <";
    dumpLocation(prefix,true, R.getBegin());
    if (R.getBegin() != R.getEnd()) {
      llvm::outs() << ", ";
      dumpLocation(prefix,false, R.getEnd());
    }
    llvm::outs() << ">";
  }

  bool VisitDecl(Decl *d) {

//    llvm::outs() << "Processing in " << srcFile << "\n";
    std::ostringstream get_the_address; 
    get_the_address << d;
    std::string address =  get_the_address.str(); 
    std::string jsonval = "";


    llvm::StringRef filename;
    SourceLocation SpellingLoc = SM->getSpellingLoc(d->getLocation());

    FullSourceLoc fullLocation = Context->getFullLoc(d->getLocation());
    if(fullLocation.hasManager())
    {
      filename = fullLocation.getManager().getFilename(fullLocation);
    } 
    if(srcFile.compare(filename.str()) == 0)
    {
       pt.put(address,jsonval);
       jsonval = address+".Type";
       pt.put(jsonval,"Decl");
       jsonval = address+".ASTname";
       pt.put(jsonval,d->getDeclKindName ());
       jsonval = address+".filename";
       pt.put(jsonval,filename.str());

       llvm::outs() << d->getDeclKindName() << "Decl";
       dumpPointer(d);
       dumpSourceRange(address+".Range",d->getSourceRange());
       llvm::outs() << " ";
       dumpLocation(address+".SpelLoc",true, SpellingLoc);
       llvm::outs() << "\n";
    }

    if(isa<DeclaratorDecl>(d))
    {
      std::ostringstream type_addr; 
      const Type* type = dyn_cast<DeclaratorDecl>(d)->getTypeSourceInfo()->getTypeLoc().getTypePtr();
      type_addr << type;
      address =  type_addr.str(); 
      TypeLoc typeLoc = dyn_cast<DeclaratorDecl>(d)->getTypeSourceInfo()->getTypeLoc();
      FullSourceLoc beginFullLocation = Context->getFullLoc(typeLoc.getBeginLoc());
      jsonval = "";
      if(srcFile.compare(filename.str()) == 0)
      {
        pt.put(address,jsonval);
        jsonval = address+".Type";
        pt.put(jsonval,"Type");
        jsonval = address+".ASTname";
        pt.put(jsonval,type->getTypeClassName());
        jsonval = address+".filename";
        pt.put(jsonval,filename.str());

        llvm::outs() << type->getTypeClassName() << "Type";
        dumpPointer(type);
        dumpSourceRange(address+".Range",typeLoc.getSourceRange());
        llvm::outs() << "\n";
      }
    }
    return true;
  }

  bool VisitDeclaratorDecl(DeclaratorDecl* dd)
  {
    std::ostringstream get_the_address; 
    get_the_address << dd;
    std::string address =  get_the_address.str(); 
    std::string jsonval = "";

    llvm::StringRef filename;
    const Type* type = dd->getTypeSourceInfo()->getTypeLoc().getTypePtr();

    TypeLoc typeLoc = dd->getTypeSourceInfo()->getTypeLoc();
    FullSourceLoc beginFullLocation = Context->getFullLoc(typeLoc.getBeginLoc());
    if(beginFullLocation.hasManager())
      filename = beginFullLocation.getManager().getFilename(beginFullLocation);
    return true;
  }


  bool VisitStmt(Stmt *s) {

    std::ostringstream get_the_address; 
    get_the_address << s;
    std::string address =  get_the_address.str(); 
    std::string jsonval = "";

    llvm::StringRef filename;

    FullSourceLoc beginFullLocation = Context->getFullLoc(s->getBeginLoc());
    filename = SM->getFilename(beginFullLocation);

    if(srcFile.compare(filename.str()) == 0)
    {
       pt.put(address,jsonval);
       jsonval = address+".Type";
       pt.put(jsonval,"Stmt");
       jsonval = address+".ASTname";
       pt.put(jsonval,s->getStmtClassName ());
       jsonval = address+".filename";
       pt.put(jsonval,filename.str());
 
       llvm::outs() << s->getStmtClassName() << "Stmt";
       dumpPointer(s);
       dumpSourceRange(address+".Range",s->getSourceRange());
       switch (s->getStmtClass()) {
         case clang::Stmt::GCCAsmStmtClass:
           dumpSourceSingleLoc(address+".AsmLoc",((clang::GCCAsmStmt*)s)->getAsmLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::GCCAsmStmt*)s)->getRParenLoc());
           break; 
         case clang::Stmt::MSAsmStmtClass:
           dumpSourceSingleLoc(address+".AsmLoc",((clang::MSAsmStmt*)s)->getAsmLoc());
           dumpSourceSingleLoc(address+".LBraceLoc",((clang::MSAsmStmt*)s)->getLBraceLoc());
           break; 
         case clang::Stmt::BreakStmtClass:
           dumpSourceSingleLoc(address+".BreakLoc",((clang::BreakStmt*)s)->getBreakLoc());
           break; 
         case clang::Stmt::CapturedStmtClass:
           break; 
         case clang::Stmt::CompoundStmtClass:
           dumpSourceSingleLoc(address+".LBracLoc",((clang::CompoundStmt*)s)->getLBracLoc());
           dumpSourceSingleLoc(address+".RBracLoc",((clang::CompoundStmt*)s)->getRBracLoc());
           break; 
         case clang::Stmt::ContinueStmtClass:
           dumpSourceSingleLoc(address+".ContinueLoc",((clang::ContinueStmt*)s)->getContinueLoc());
           break; 
         case clang::Stmt::CoreturnStmtClass:
         case clang::Stmt::CoroutineBodyStmtClass:
           break; 
         case clang::Stmt::CXXCatchStmtClass:
           dumpSourceSingleLoc(address+".CatchLoc",((clang::CXXCatchStmt*)s)->getCatchLoc());
           break; 
         case clang::Stmt::CXXForRangeStmtClass:
           dumpSourceSingleLoc(address+".ForLoc",((clang::CXXForRangeStmt*)s)->getForLoc());
           dumpSourceSingleLoc(address+".CoawaitLoc",((clang::CXXForRangeStmt*)s)->getCoawaitLoc());
           dumpSourceSingleLoc(address+".ColonLoc",((clang::CXXForRangeStmt*)s)->getColonLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::CXXForRangeStmt*)s)->getRParenLoc());
           break; 
         case clang::Stmt::CXXTryStmtClass:
           dumpSourceSingleLoc(address+".TryLoc",((clang::CXXTryStmt*)s)->getTryLoc());
           break; 
         case clang::Stmt::DeclStmtClass:
           break; 
         case clang::Stmt::DoStmtClass:
           dumpSourceSingleLoc(address+".DoLoc",((clang::DoStmt*)s)->getDoLoc());
           dumpSourceSingleLoc(address+".WhileLoc",((clang::DoStmt*)s)->getWhileLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::DoStmt*)s)->getRParenLoc());
           break; 
         case clang::Stmt::ForStmtClass:
           dumpSourceSingleLoc(address+".ForLoc",((clang::ForStmt*)s)->getForLoc());
           dumpSourceSingleLoc(address+".LParenLoc",((clang::ForStmt*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::ForStmt*)s)->getRParenLoc());
           break; 
         case clang::Stmt::GotoStmtClass:
           dumpSourceSingleLoc(address+".GotoLoc",((clang::GotoStmt*)s)->getGotoLoc());
           dumpSourceSingleLoc(address+".LabelLoc",((clang::GotoStmt*)s)->getLabelLoc());
           break; 
         case clang::Stmt::IfStmtClass:
           dumpSourceSingleLoc(address+".IfLoc",((clang::IfStmt*)s)->getIfLoc());
           dumpSourceSingleLoc(address+".ElseLoc",((clang::IfStmt*)s)->getElseLoc());
           dumpSourceSingleLoc(address+".LParenLoc",((clang::IfStmt*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::IfStmt*)s)->getRParenLoc());
           break; 
         case clang::Stmt::IndirectGotoStmtClass:
           dumpSourceSingleLoc(address+".GotoLoc",((clang::IndirectGotoStmt*)s)->getGotoLoc());
           dumpSourceSingleLoc(address+".StarLoc",((clang::IndirectGotoStmt*)s)->getStarLoc());
           break; 
         case clang::Stmt::MSDependentExistsStmtClass:
           dumpSourceSingleLoc(address+".KeywordLoc",((clang::MSDependentExistsStmt*)s)->getKeywordLoc());
           break; 
         case clang::Stmt::NullStmtClass:
           dumpSourceSingleLoc(address+".SemiLoc",((clang::NullStmt*)s)->getSemiLoc());
           break; 
         case clang::Stmt::OMPAtomicDirectiveClass:
         case clang::Stmt::OMPBarrierDirectiveClass:
         case clang::Stmt::OMPCancelDirectiveClass:
         case clang::Stmt::OMPCancellationPointDirectiveClass:
         case clang::Stmt::OMPCriticalDirectiveClass:
         case clang::Stmt::OMPDepobjDirectiveClass:
         case clang::Stmt::OMPFlushDirectiveClass:
         case clang::Stmt::OMPDispatchDirectiveClass:
         case clang::Stmt::OMPInteropDirectiveClass:
         case clang::Stmt::OMPDistributeDirectiveClass:
         case clang::Stmt::OMPDistributeParallelForDirectiveClass:
         case clang::Stmt::OMPDistributeParallelForSimdDirectiveClass:
         case clang::Stmt::OMPDistributeSimdDirectiveClass:
         case clang::Stmt::OMPForDirectiveClass:
         case clang::Stmt::OMPForSimdDirectiveClass:
         case clang::Stmt::OMPParallelForDirectiveClass:
         case clang::Stmt::OMPParallelForSimdDirectiveClass:
         case clang::Stmt::OMPGenericLoopDirectiveClass:
         //case clang::Stmt::OMPMaskedTaskLoopDirectiveClass:
         case clang::Stmt::OMPMasterTaskLoopDirectiveClass:
         //case clang::Stmt::OMPParallelGenericLoopDirectiveClass:
         case clang::Stmt::OMPParallelMasterTaskLoopDirectiveClass:
         case clang::Stmt::OMPMasterTaskLoopSimdDirectiveClass:
         case clang::Stmt::OMPSimdDirectiveClass:
         case clang::Stmt::OMPTargetParallelForDirectiveClass:
         case clang::Stmt::OMPTargetSimdDirectiveClass:
         case clang::Stmt::OMPTargetTeamsDistributeDirectiveClass:
         case clang::Stmt::OMPTargetTeamsDistributeParallelForDirectiveClass:
         case clang::Stmt::OMPTargetTeamsDistributeSimdDirectiveClass:
         //case clang::Stmt::OMPTargetTeamsGenericLoopDirectiveClass:
         case clang::Stmt::OMPTeamsDistributeParallelForSimdDirectiveClass:
         //case clang::Stmt::OMPTeamsGenericLoopDirectiveClass:
         case clang::Stmt::OMPTileDirectiveClass:
         case clang::Stmt::OMPUnrollDirectiveClass:
         case clang::Stmt::OMPCanonicalLoopClass:
           break;
         case clang::Stmt::ReturnStmtClass:
           dumpSourceSingleLoc(address+".ReturnLoc",((clang::ReturnStmt*)s)->getReturnLoc());
           break;
         case clang::Stmt::SEHExceptStmtClass:
           dumpSourceSingleLoc(address+".ExceptLoc",((clang::SEHExceptStmt*)s)->getExceptLoc());
           break;
         case clang::Stmt::SEHFinallyStmtClass:
           dumpSourceSingleLoc(address+".FinallyLoc",((clang::SEHFinallyStmt*)s)->getFinallyLoc());
           break;
         case clang::Stmt::SEHLeaveStmtClass:
           dumpSourceSingleLoc(address+".LeaveLoc",((clang::SEHLeaveStmt*)s)->getLeaveLoc());
           break;
         case clang::Stmt::SEHTryStmtClass:
           dumpSourceSingleLoc(address+".TryLoc",((clang::SEHTryStmt*)s)->getTryLoc());
           break;
         case clang::Stmt::CaseStmtClass:
           dumpSourceSingleLoc(address+".CaseLoc",((clang::CaseStmt*)s)->getCaseLoc());
           break;
         case clang::Stmt::DefaultStmtClass:
           dumpSourceSingleLoc(address+".DefaultLoc",((clang::DefaultStmt*)s)->getDefaultLoc());
           break;
         case clang::Stmt::SwitchStmtClass:
           dumpSourceSingleLoc(address+".SwitchLoc",((clang::SwitchStmt*)s)->getSwitchLoc());
           dumpSourceSingleLoc(address+".LParenLoc",((clang::SwitchStmt*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::SwitchStmt*)s)->getRParenLoc());
           break;
         case clang::Stmt::AttributedStmtClass:
           dumpSourceSingleLoc(address+".AttrLoc",((clang::AttributedStmt*)s)->getAttrLoc());
           break;
         case clang::Stmt::BinaryConditionalOperatorClass:
         case clang::Stmt::ConditionalOperatorClass:
           break;
         case clang::Stmt::AddrLabelExprClass:
           dumpSourceSingleLoc(address+".AmpAmpLoc",((clang::AddrLabelExpr*)s)->getAmpAmpLoc());
           dumpSourceSingleLoc(address+".LabelLoc",((clang::AddrLabelExpr*)s)->getLabelLoc());
           break;
         case clang::Stmt::ArrayInitIndexExprClass:
         case clang::Stmt::ArrayInitLoopExprClass:
           break;
         case clang::Stmt::ArraySubscriptExprClass:
           dumpSourceSingleLoc(address+".RBracketLoc",((clang::ArraySubscriptExpr*)s)->getRBracketLoc());
           break;
         case clang::Stmt::ArrayTypeTraitExprClass:
           break;
         case clang::Stmt::AsTypeExprClass:
           dumpSourceSingleLoc(address+".BuiltinLoc",((clang::AsTypeExpr*)s)->getBuiltinLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::AsTypeExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::AtomicExprClass:
           dumpSourceSingleLoc(address+".BuiltinLoc",((clang::AtomicExpr*)s)->getBuiltinLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::AtomicExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::BinaryOperatorClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::BinaryOperator*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::BinaryOperator*)s)->getExprLoc());
           break; 
         case clang::Stmt::BlockExprClass:
           dumpSourceSingleLoc(address+".CaretLoc",((clang::BlockExpr*)s)->getCaretLocation());
           break; 
         case clang::Stmt::CUDAKernelCallExprClass:
           break;
         case clang::Stmt::CXXMemberCallExprClass:
           dumpSourceSingleLoc(address+".ExprLoc",((clang::CXXMemberCallExpr*)s)->getExprLoc());
           break;
         case clang::Stmt::CXXOperatorCallExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::CXXOperatorCallExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::CXXOperatorCallExpr*)s)->getExprLoc());
           break;
         case clang::Stmt::UserDefinedLiteralClass:
           dumpSourceSingleLoc(address+".UDSuffixLoc",((clang::UserDefinedLiteral*)s)->getUDSuffixLoc());
           break;
         case clang::Stmt::BuiltinBitCastExprClass:
           break;
         case clang::Stmt::CStyleCastExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::CStyleCastExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::CStyleCastExpr*)s)->getRParenLoc());
           break;
        case clang::Stmt::CXXFunctionalCastExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::CXXFunctionalCastExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::CXXFunctionalCastExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::CXXAddrspaceCastExprClass:
         case clang::Stmt::CXXConstCastExprClass:
         case clang::Stmt::CXXDynamicCastExprClass:
         case clang::Stmt::CXXReinterpretCastExprClass:
         case clang::Stmt::CXXStaticCastExprClass:
         case clang::Stmt::ImplicitCastExprClass:
           break;
         case clang::Stmt::CharacterLiteralClass:
//           dumpSourceSingleLoc(address+".Location",((clang::CharacterLiteral*)s)->getLocation());
           break;
         case clang::Stmt::ChooseExprClass:
           dumpSourceSingleLoc(address+".BuiltinLoc",((clang::ChooseExpr*)s)->getBuiltinLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::ChooseExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::CompoundLiteralExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::CompoundLiteralExpr*)s)->getLParenLoc());
           break;
         case clang::Stmt::ConceptSpecializationExprClass:
           break;
         case clang::Stmt::ConvertVectorExprClass:
           dumpSourceSingleLoc(address+".BuiltinLoc",((clang::ConvertVectorExpr*)s)->getBuiltinLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::ConvertVectorExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::CoawaitExprClass:
         case clang::Stmt::CoyieldExprClass:
         case clang::Stmt::CXXBindTemporaryExprClass:
           break;
         case clang::Stmt::CXXBoolLiteralExprClass:
//           dumpSourceSingleLoc(address+".Location",((clang::CXXBoolLiteralExpr*)s)->getLocation());
           break;
         case clang::Stmt::CXXTemporaryObjectExprClass:
           break;
         case clang::Stmt::CXXDefaultArgExprClass:
           dumpSourceSingleLoc(address+".UsedLocation",((clang::CXXDefaultArgExpr*)s)->getUsedLocation());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::CXXDefaultArgExpr*)s)->getExprLoc());
           break;
         case clang::Stmt::CXXDefaultInitExprClass:
           dumpSourceSingleLoc(address+".UsedLocation",((clang::CXXDefaultInitExpr*)s)->getUsedLocation());
           break;
         case clang::Stmt::CXXDeleteExprClass:
           break;
         case clang::Stmt::CXXDependentScopeMemberExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::CXXDependentScopeMemberExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".MemberLoc",((clang::CXXDependentScopeMemberExpr*)s)->getMemberLoc());
           dumpSourceSingleLoc(address+".TemplateKeywordLoc",((clang::CXXDependentScopeMemberExpr*)s)->getTemplateKeywordLoc());
           dumpSourceSingleLoc(address+".LAngle",((clang::CXXDependentScopeMemberExpr*)s)->getLAngleLoc());
           dumpSourceSingleLoc(address+".RAngle",((clang::CXXDependentScopeMemberExpr*)s)->getRAngleLoc());
           break;
         case clang::Stmt::CXXFoldExprClass:
           dumpSourceSingleLoc(address+".LParen",((clang::CXXFoldExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParen",((clang::CXXFoldExpr*)s)->getRParenLoc());
           dumpSourceSingleLoc(address+".Ellipsis",((clang::CXXFoldExpr*)s)->getEllipsisLoc());
           break;
         case clang::Stmt::CXXInheritedCtorInitExprClass:
//           dumpSourceSingleLoc(address+".Location",((clang::CXXInheritedCtorInitExpr*)s)->getLocation());
           break;
         case clang::Stmt::CXXNewExprClass:
             dumpSourceRange(address+".DirectInitRange",((clang::CXXNewExpr*)s)->getDirectInitRange());
           break;
         case clang::Stmt::CXXNoexceptExprClass:
         case clang::Stmt::CXXNullPtrLiteralExprClass:
           break;
         case clang::Stmt::CXXPseudoDestructorExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::CXXPseudoDestructorExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".ColonColonLoc",((clang::CXXPseudoDestructorExpr*)s)->getColonColonLoc());
           dumpSourceSingleLoc(address+".TildeLoc",((clang::CXXPseudoDestructorExpr*)s)->getTildeLoc());
           dumpSourceSingleLoc(address+".DestroyedTypeLoc",((clang::CXXPseudoDestructorExpr*)s)->getDestroyedTypeLoc());
           break;
         case clang::Stmt::CXXRewrittenBinaryOperatorClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::CXXRewrittenBinaryOperator*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::CXXRewrittenBinaryOperator*)s)->getExprLoc());
           break;
         case clang::Stmt::CXXScalarValueInitExprClass:
           dumpSourceSingleLoc(address+".RParenLoc",((clang::CXXScalarValueInitExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::CXXStdInitializerListExprClass:
           break;
         case clang::Stmt::CXXThisExprClass:
//           dumpSourceSingleLoc(address+".Location",((clang::CXXThisExpr*)s)->getLocation());
           break;
         case clang::Stmt::CXXThrowExprClass:
           dumpSourceSingleLoc(address+".ThrowLoc",((clang::CXXThrowExpr*)s)->getThrowLoc());
           break;
         case clang::Stmt::CXXTypeidExprClass:
           break;
         case clang::Stmt::CXXUnresolvedConstructExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::CXXUnresolvedConstructExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::CXXUnresolvedConstructExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::CXXUuidofExprClass:
           break;
         case clang::Stmt::DeclRefExprClass:
//           dumpSourceSingleLoc(address+".Location",((clang::DeclRefExpr*)s)->getLocation());
           dumpSourceSingleLoc(address+".TemplateKeywordLoc",((clang::DeclRefExpr*)s)->getTemplateKeywordLoc());
           dumpSourceSingleLoc(address+".LAngleLoc",((clang::DeclRefExpr*)s)->getLAngleLoc());
           dumpSourceSingleLoc(address+".RAngleLoc",((clang::DeclRefExpr*)s)->getRAngleLoc());
           break;
         case clang::Stmt::DependentCoawaitExprClass:
           dumpSourceSingleLoc(address+".KeywordLoc",((clang::DependentCoawaitExpr*)s)->getKeywordLoc());
           break;
        case clang::Stmt::DependentScopeDeclRefExprClass:
//           dumpSourceSingleLoc(address+".Location",((clang::DependentScopeDeclRefExpr*)s)->getLocation());
           dumpSourceSingleLoc(address+".TemplateKeywordLoc",((clang::DependentScopeDeclRefExpr*)s)->getTemplateKeywordLoc());
           dumpSourceSingleLoc(address+".LAngleLoc",((clang::DependentScopeDeclRefExpr*)s)->getLAngleLoc());
           dumpSourceSingleLoc(address+".RAngleLoc",((clang::DependentScopeDeclRefExpr*)s)->getRAngleLoc());
           break;
         case clang::Stmt::DesignatedInitExprClass:
           dumpSourceSingleLoc(address+".EqualOrColonLoc",((clang::DesignatedInitExpr*)s)->getEqualOrColonLoc());
           dumpSourceRange(address+".DesignatorSSourceRange",((clang::DesignatedInitExpr*)s)->getDesignatorsSourceRange());
           break;
         case clang::Stmt::DesignatedInitUpdateExprClass:
           break;
         case clang::Stmt::LabelStmtClass:
           dumpSourceSingleLoc(address+".IdentLoc",((clang::LabelStmt*)s)->getIdentLoc());
           break;
         case clang::Stmt::ExpressionTraitExprClass:
           break;
         case clang::Stmt::ExtVectorElementExprClass:
           dumpSourceSingleLoc(address+".AccessorLoc",((clang::ExtVectorElementExpr*)s)->getAccessorLoc());
           break;
         case clang::Stmt::FixedPointLiteralClass:
         case clang::Stmt::FloatingLiteralClass:
         case clang::Stmt::ConstantExprClass:
         case clang::Stmt::ExprWithCleanupsClass:
           break;
         case clang::Stmt::FunctionParmPackExprClass:
           dumpSourceSingleLoc(address+".ParameterPackLoc",((clang::FunctionParmPackExpr*)s)->getParameterPackLocation());
           break;
         case clang::Stmt::GenericSelectionExprClass:
           dumpSourceSingleLoc(address+".GenericLoc",((clang::GenericSelectionExpr*)s)->getGenericLoc());
           dumpSourceSingleLoc(address+".DefaultLoc",((clang::GenericSelectionExpr*)s)->getDefaultLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::GenericSelectionExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::GNUNullExprClass:
           dumpSourceSingleLoc(address+".TokenLoc",((clang::GNUNullExpr*)s)->getTokenLocation());
           break;
         case clang::Stmt::ImaginaryLiteralClass:
         case clang::Stmt::ImplicitValueInitExprClass:
           break;
         case clang::Stmt::InitListExprClass:
           dumpSourceSingleLoc(address+".LBraceLoc",((clang::InitListExpr*)s)->getLBraceLoc());
           dumpSourceSingleLoc(address+".RBraceLoc",((clang::InitListExpr*)s)->getRBraceLoc());
           break;
         case clang::Stmt::IntegerLiteralClass:
           break;
         case clang::Stmt::LambdaExprClass:
           dumpSourceSingleLoc(address+".CaptureDefaultLoc",((clang::LambdaExpr*)s)->getCaptureDefaultLoc());
           dumpSourceRange(address+".IntroducerRange",((clang::LambdaExpr*)s)->getIntroducerRange());
           break;
         case clang::Stmt::MaterializeTemporaryExprClass:
           break;
         case clang::Stmt::MatrixSubscriptExprClass:
           dumpSourceSingleLoc(address+".RBracketLoc",((clang::MatrixSubscriptExpr*)s)->getRBracketLoc());
           break;
         case clang::Stmt::MemberExprClass:
           dumpSourceSingleLoc(address+".TemplateKeywordLoc",((clang::MemberExpr*)s)->getTemplateKeywordLoc());
           dumpSourceSingleLoc(address+".LAngleLoc",((clang::MemberExpr*)s)->getLAngleLoc());
           dumpSourceSingleLoc(address+".RAngleLoc",((clang::MemberExpr*)s)->getRAngleLoc());
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::MemberExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".MemberLoc",((clang::MemberExpr*)s)->getMemberLoc());
           break;
         case clang::Stmt::MSPropertyRefExprClass:
           dumpSourceSingleLoc(address+".MemberLoc",((clang::MSPropertyRefExpr*)s)->getMemberLoc());
           break;
         case clang::Stmt::MSPropertySubscriptExprClass:
           dumpSourceSingleLoc(address+".RBracketLoc",((clang::MSPropertySubscriptExpr*)s)->getRBracketLoc());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::MSPropertySubscriptExpr*)s)->getExprLoc());
           break;
         case clang::Stmt::NoInitExprClass:
           break;
         case clang::Stmt::OffsetOfExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::OffsetOfExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::OffsetOfExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::OMPArraySectionExprClass:
           dumpSourceSingleLoc(address+".ColonLocFirst",((clang::OMPArraySectionExpr*)s)->getColonLocFirst());
           dumpSourceSingleLoc(address+".ColonLocSecond",((clang::OMPArraySectionExpr*)s)->getColonLocSecond());
           dumpSourceSingleLoc(address+".RBracketLoc",((clang::OMPArraySectionExpr*)s)->getRBracketLoc());
           break;
         case clang::Stmt::OMPArrayShapingExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::OMPArrayShapingExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::OMPArrayShapingExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::OMPIteratorExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::OMPIteratorExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::OMPIteratorExpr*)s)->getRParenLoc());
           dumpSourceSingleLoc(address+".IteratorKwLoc",((clang::OMPIteratorExpr*)s)->getIteratorKwLoc());
           break;
         case clang::Stmt::OpaqueValueExprClass:
           dumpSourceSingleLoc(address+".ExprLoc",((clang::OpaqueValueExpr*)s)->getExprLoc());
           break;
         case clang::Stmt::UnresolvedLookupExprClass:
           break;
         case clang::Stmt::UnresolvedMemberExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::UnresolvedMemberExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".MemberLoc",((clang::UnresolvedMemberExpr*)s)->getMemberLoc());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::UnresolvedMemberExpr*)s)->getExprLoc());
           break;
         case clang::Stmt::PackExpansionExprClass:
           dumpSourceSingleLoc(address+".Ellipsis",((clang::PackExpansionExpr*)s)->getEllipsisLoc());
           break;
         case clang::Stmt::ParenExprClass:
           dumpSourceSingleLoc(address+".LParen",((clang::ParenExpr*)s)->getLParen());
           break;
         case clang::Stmt::ParenListExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::ParenListExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::ParenListExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::PredefinedExprClass:
           break;
        case clang::Stmt::PseudoObjectExprClass:
           dumpSourceSingleLoc(address+".ExprLoc",((clang::PseudoObjectExpr*)s)->getExprLoc());
           break;
        case clang::Stmt::RecoveryExprClass:
           break;
        case clang::Stmt::RequiresExprClass:
           dumpSourceSingleLoc(address+".RequiresKWLoc",((clang::RequiresExpr*)s)->getRequiresKWLoc());
           dumpSourceSingleLoc(address+".RBraceLoc",((clang::RequiresExpr*)s)->getRBraceLoc());
           break;
        case clang::Stmt::ShuffleVectorExprClass:
           dumpSourceSingleLoc(address+".BuiltinLoc",((clang::ShuffleVectorExpr*)s)->getBuiltinLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::ShuffleVectorExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::SizeOfPackExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::SizeOfPackExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".PackLoc",((clang::SizeOfPackExpr*)s)->getPackLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::SizeOfPackExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::SourceLocExprClass:
           break;
         case clang::Stmt::StmtExprClass:
           dumpSourceSingleLoc(address+".LParenLoc",((clang::StmtExpr*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::StmtExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::StringLiteralClass:
           break;
         case clang::Stmt::SubstNonTypeTemplateParmExprClass:
           dumpSourceSingleLoc(address+".NameLoc",((clang::SubstNonTypeTemplateParmExpr*)s)->getNameLoc());
           break;
         case clang::Stmt::SubstNonTypeTemplateParmPackExprClass:
           dumpSourceSingleLoc(address+".ParameterPackLocation",((clang::SubstNonTypeTemplateParmPackExpr*)s)->getParameterPackLocation());
           break;
         case clang::Stmt::TypeTraitExprClass:
         case clang::Stmt::TypoExprClass:
           break;
         case clang::Stmt::UnaryExprOrTypeTraitExprClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::UnaryExprOrTypeTraitExpr*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::UnaryExprOrTypeTraitExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::UnaryOperatorClass:
           dumpSourceSingleLoc(address+".OperatorLoc",((clang::UnaryOperator*)s)->getOperatorLoc());
           dumpSourceSingleLoc(address+".ExprLoc",((clang::UnaryOperator*)s)->getExprLoc());
           break;
         case clang::Stmt::VAArgExprClass:
           dumpSourceSingleLoc(address+".BuiltinLoc",((clang::VAArgExpr*)s)->getBuiltinLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::VAArgExpr*)s)->getRParenLoc());
           break;
         case clang::Stmt::WhileStmtClass:
           dumpSourceSingleLoc(address+".WhileLoc",((clang::WhileStmt*)s)->getWhileLoc());
           dumpSourceSingleLoc(address+".LParenLoc",((clang::WhileStmt*)s)->getLParenLoc());
           dumpSourceSingleLoc(address+".RParenLoc",((clang::WhileStmt*)s)->getRParenLoc());
           break;
         case clang::Stmt::NoStmtClass:
         default:
           llvm::outs() << " Unchecked statement kind: \n";
       }
       llvm::outs() << "\n";
    }
    return true;
  }

private:
  ASTContext *Context;
  std::string srcFile;
  const SourceManager *SM = nullptr;
};


class RoseConnectionClangPluginConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  llvm::StringRef inFile;

public:
  RoseConnectionClangPluginConsumer(CompilerInstance &Instance, llvm::StringRef inputFile)
      : Instance(Instance), visitor(&Instance.getASTContext(), inputFile.str()), inFile(inputFile) {}

  void Initialize(ASTContext &Context) override {
    SourceManager& SrcMgr = Context.getSourceManager();
    llvm::errs() << "my customized initizlier for input file: " << inFile << "\"\n";

// The following is needed when ROSE is included  
/*    
    ROSE_INITIALIZE;
    std::vector<std::string> roseFEArgs{"","-rose:skipfinalCompileStep",inFile.str()};
    project = ::frontend(roseFEArgs); 
    std::cout << "In Plugin: generated sgproject:" << project << std::endl;
*/
 }

 virtual void HandleTranslationUnit(clang::ASTContext &Context) override{
   visitor.TraverseDecl(Context.getTranslationUnitDecl());
   std::stringstream ss;
   boost::property_tree::json_parser::write_json(ss, pt);
   std::ofstream jfile;
   jfile.open("srcLoc.json",std::ios::out);
   jfile << ss.rdbuf();
   jfile.close();
 }

private:
  collectInfoVisitor visitor;
};

class RoseConnectionClangPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef inFile) override {
    return std::make_unique<RoseConnectionClangPluginConsumer>(CI, inFile);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "RoseConnectionClangPlugin arg = " << args[i] << "\n";

      // Example error handling.
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-an-error") {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "invalid argument '%0'");
        D.Report(DiagID) << args[i];
        return false;
      } else if (args[i] == "-parse-template") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -parse-template argument"));
          return false;
        }
        ++i;
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }


  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for RoseConnectionClangPlugin plugin goes here\n";
  }

  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }

};

}

static FrontendPluginRegistry::Add<RoseConnectionClangPluginAction>
X("ROSE", "call ROSE");
