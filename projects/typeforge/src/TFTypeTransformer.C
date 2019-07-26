#include "sage3basic.h"
#include "TFTypeTransformer.h"
#include "SgNodeHelper.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "CppStdUtilities.h"
#include "ToolConfig.hpp"

using namespace std;

#ifndef DEBUG__statics__addExplicitCast
#  define DEBUG__statics__addExplicitCast 0
#endif
#ifndef DEBUG__TFTypeTransformer
#  define DEBUG__TFTypeTransformer 0
#endif
#ifndef DEBUG__TFTypeTransformer__rebuildBaseType
#  define DEBUG__TFTypeTransformer__rebuildBaseType DEBUG__TFTypeTransformer
#endif
#ifndef DEBUG__TFTypeTransformer__changeType
#  define DEBUG__TFTypeTransformer__changeType DEBUG__TFTypeTransformer
#endif
#ifndef DEBUG__TFTypeTransformer_changeVariableType
#  define DEBUG__TFTypeTransformer_changeVariableType DEBUG__TFTypeTransformer
#endif

namespace Typeforge {

static void addExplicitCast() {

#if DEBUG__statics__addExplicitCast
  std::cout << "ENTER addExplicitCast" << std::endl;
#endif

  RoseAst ast(::Typeforge::project);
  for (auto n: ast) {
    SgBinaryOp * bop = isSgBinaryOp(n);
    if (bop == NULL) continue;
    SgAssignOp * aop = isSgAssignOp(n);
    SgCompoundAssignOp * caop = isSgCompoundAssignOp(n);
    if (aop != NULL || caop != NULL) {
      SgExpression * lhs = bop->get_lhs_operand_i();
      SgType * lhs_t = lhs->get_type();
      SgType * lhs_t_s = lhs_t->stripType(
        SgType::STRIP_ARRAY_TYPE     |
        SgType::STRIP_POINTER_TYPE   |
        SgType::STRIP_MODIFIER_TYPE  |
        SgType::STRIP_REFERENCE_TYPE |
        SgType::STRIP_RVALUE_REFERENCE_TYPE |
        SgType::STRIP_TYPEDEF_TYPE
      );
      SgExpression * rhs = bop->get_rhs_operand_i();
      SgType * rhs_t = rhs->get_type();
      SgType * rhs_t_s = rhs_t->stripType(
        SgType::STRIP_ARRAY_TYPE     |
        SgType::STRIP_POINTER_TYPE   |
        SgType::STRIP_MODIFIER_TYPE  |
        SgType::STRIP_REFERENCE_TYPE |
        SgType::STRIP_RVALUE_REFERENCE_TYPE |
        SgType::STRIP_TYPEDEF_TYPE
      );

      if (isSgPointerType(lhs_t) && (
            isSgTypeShort(rhs_t) || isSgTypeInt(rhs_t) || isSgTypeLong(rhs_t) || isSgTypeLongLong(rhs_t) ||
            isSgTypeUnsignedShort(rhs_t) || isSgTypeUnsignedInt(rhs_t) || isSgTypeUnsignedLong(rhs_t) || isSgTypeUnsignedLongLong(rhs_t)
      ) ) {
        continue; // int cast to pointer types (think increasing pointer)
      }

      if (rhs_t_s != lhs_t_s) {
        SgType * cast_type = lhs_t->stripType(
          SgType::STRIP_MODIFIER_TYPE  |
          SgType::STRIP_REFERENCE_TYPE |
          SgType::STRIP_RVALUE_REFERENCE_TYPE
        );
#if DEBUG__statics__addExplicitCast
        std::cout << " * add cast:" << std::endl;
        std::cout << "      bop       = " << bop       << " ( " << bop->class_name()       << ")"   << std::endl;
        std::cout << "      lhs       = " << lhs       << " ( " << lhs->class_name()       << ")"   << std::endl;
        std::cout << "      lhs_t     = " << lhs_t     << " ( " << lhs_t->class_name()     << "): " << lhs_t->unparseToString()     << "" << std::endl;
        std::cout << "      lhs_t_s   = " << lhs_t_s   << " ( " << lhs_t_s->class_name()   << "): " << lhs_t_s->unparseToString()   << "" << std::endl;
        std::cout << "      rhs       = " << rhs       << " ( " << rhs->class_name()       << ")"   << std::endl;
        std::cout << "      rhs_t     = " << rhs_t     << " ( " << rhs_t->class_name()     << "): " << rhs_t->unparseToString()     << "" << std::endl;
        std::cout << "      rhs_t_s   = " << rhs_t_s   << " ( " << rhs_t_s->class_name()   << "): " << rhs_t_s->unparseToString()   << "" << std::endl;
        std::cout << "      cast_type = " << cast_type << " ( " << cast_type->class_name() << "): " << cast_type->unparseToString() << "" << std::endl;
#endif
        SgExpression * new_rhs = SageBuilder::buildCastExp(rhs, cast_type);
        rhs->set_parent(new_rhs);
        new_rhs->set_parent(bop);
        bop->set_rhs_operand_i(new_rhs);
      }
    }
  }
}

//returns a new type with same structure as root but with newBaseType as a base
static SgType * rebuildBaseType(SgType* root, SgType* newBaseType) {

#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "TFTypeTransformer::rebuildBaseType" << std::endl;
  std::cout << "  root        = " << root        << " ( " << root->class_name()        << "): " << root->unparseToString()        << "" << std::endl;
  std::cout << "  newBaseType = " << newBaseType << " ( " << newBaseType->class_name() << "): " << newBaseType->unparseToString() << "" << std::endl;
#endif

  SgType * new_type = nullptr;

  if (SgArrayType* arrayType = isSgArrayType(root)) {
    // handle array type
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "> array" << std::endl;
#endif
    SgType* base = rebuildBaseType(arrayType->get_base_type(), newBaseType);
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "< array" << std::endl;
#endif
    SgExpression* index = arrayType->get_index();
    SgExprListExp* dim_info = arrayType->get_dim_info();
    if (dim_info != nullptr) {
      new_type = SageBuilder::buildArrayType(base, dim_info);
    } else if(index != nullptr) {
      new_type = SageBuilder::buildArrayType(base, index);
    } else {
      new_type = SageBuilder::buildArrayType(base);
    }

  } else if(SgPointerType* pointerType = isSgPointerType(root)) {
    // handle pointer type
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "> pointer" << std::endl;
#endif
    SgType* base = rebuildBaseType(pointerType->get_base_type(), newBaseType);
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "< pointer" << std::endl;
#endif
    SgPointerType* newPointer = SageBuilder::buildPointerType(base);
    new_type = newPointer;

  } else if(SgTypedefType* defType = isSgTypedefType(root)) {
    //  handle typedef, does not build new typedef. builds type around structure defined in typedef
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "> typedef" << std::endl;
#endif
    new_type = rebuildBaseType(defType->get_base_type(), newBaseType);
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "< typedef" << std::endl;
#endif

  } else if(SgReferenceType* refType = isSgReferenceType(root)) {
    //  handle reference type
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "> reference" << std::endl;
#endif
    SgType* base = rebuildBaseType(refType->get_base_type(), newBaseType);
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "< reference" << std::endl;
#endif
    SgReferenceType* newReference = SageBuilder::buildReferenceType(base);
    new_type = newReference;

  } else if(SgModifierType* modType = isSgModifierType(root)) {
    // handle type modifiers(const, restrict, volatile)
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "> modifier" << std::endl;
#endif
    SgType* base =  rebuildBaseType(modType->get_base_type(), newBaseType);
#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "< modifier" << std::endl;
#endif
    SgTypeModifier modifier = modType->get_typeModifier();
    if (modifier.isRestrict()) {
      new_type = SageBuilder::buildRestrictType(base);
    } else {
      SgConstVolatileModifier cmod = modifier.get_constVolatileModifier();
      if (cmod.isConst()) {
        new_type = SageBuilder::buildConstType(base);
      } else if(cmod.isVolatile()) {
        new_type = SageBuilder::buildVolatileType(base);
      } else {
        new_type = SageBuilder::buildModifierType(base);
      }
    }

  } else if (SgClassType * xtype = isSgClassType(root)) {
    SgDeclarationStatement * decl_stmt = xtype->get_declaration();
    assert(decl_stmt != nullptr);

    SgTemplateInstantiationDecl * ti_decl = isSgTemplateInstantiationDecl(decl_stmt);
    assert(ti_decl != nullptr);

    SgTemplateClassDeclaration * td_decl = ti_decl->get_templateDeclaration();
    assert(td_decl != nullptr);

    assert(td_decl->get_qualified_name() == "::std::vector");

//  std:ostringstream oss; oss << "vector< " << newBaseType->unparseToString() << ; SgName new_inst_name(oss.str());
    SgName new_inst_name("vector");

    std::vector<SgTemplateArgument *> tpl_args = ti_decl->get_templateArguments();
    assert(tpl_args.size() > 0);
    assert(tpl_args[0] != nullptr);
    tpl_args[0]->set_type(newBaseType);
    tpl_args.erase(tpl_args.begin()+1, tpl_args.end());

    SgClassDeclaration * new_xdecl = SageBuilder::buildNondefiningClassDeclaration_nfi(
        new_inst_name, ti_decl->get_class_type(), ti_decl->get_scope(), true, &tpl_args
    );

    SgTemplateInstantiationDecl * new_ti_decl = isSgTemplateInstantiationDecl(new_xdecl);
    assert(new_ti_decl != nullptr);

    assert(new_ti_decl->get_type() != nullptr);

    new_type = new_ti_decl->get_type();
  } else {
    // reached base so return new base instead
    new_type = newBaseType;
  }

  assert(new_type != nullptr);

#if DEBUG__TFTypeTransformer__rebuildBaseType
  std::cout << "  new_type    = " << new_type << " ( " << new_type->class_name() << "): " << new_type->unparseToString() << "" << std::endl;
#endif

  return new_type;
}

///////////////////////////////////////////////////////////////////////////

void makeAllCastsExplicit() {
  RoseAst ast(::Typeforge::project);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCastExp* castExp=isSgCastExp(*i)) {
      if(castExp->isCompilerGenerated()) {
	castExp->unsetCompilerGenerated();
      }
    }
  }
}

void annotateImplicitCastsAsComments() {
  RoseAst ast(::Typeforge::project);
  string matchexpression="$CastNode=SgCastExp($CastOpChild)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,::Typeforge::project);
  //cout << "Number of matched patterns with bound variables: " << r.size() << endl;
  list<string> report;
  int statementTransformations=0;
  for(MatchResult::reverse_iterator i=r.rbegin();i!=r.rend();++i) {
    statementTransformations++;
    SgCastExp* castExp=isSgCastExp((*i)["$CastNode"]);
    ROSE_ASSERT(castExp);
    SgExpression* childNode=isSgExpression((*i)["$CastOpChild"]);
    ROSE_ASSERT(childNode);
    if(castExp->isCompilerGenerated()) {
      SgType* castType=castExp->get_type();
      string castTypeString=castType->unparseToString();
      SgType* castedType=childNode->get_type();
      string castedTypeString=castedType->unparseToString();
      string reportLine="compiler generated cast: "
        +SgNodeHelper::sourceLineColumnToString(castExp->get_parent())
        +": "+castTypeString+" <== "+castedTypeString;
      if(castType==castedType) {
        reportLine+=" [ no change in type. ]";
      }
      // line are created in reverse order
      report.push_front(reportLine); 
      
      string newSourceCode;
      newSourceCode="/*CAST("+castTypeString+")*/";
      newSourceCode+=castExp->unparseToString();
      castExp->unsetCompilerGenerated(); // otherwise it is not replaced
      SgNodeHelper::replaceAstWithString(castExp,newSourceCode);
    }
  }
  for(list<string>::iterator i=report.begin();i!=report.end();++i) {
    cout<<*i<<endl;
  }
  //m.printMarkedLocations();
  //m.printMatchOperationsSequence();
  cout<<"Number of compiler generated casts: "<<statementTransformations<<endl;
}

//////////////////////////////////////////////////////////////////////////////////////

void TFTypeTransformer::addToActionList(SgNode* node, SgType* toType, bool base) {

  assert(node && toType);

  if(base) {
    ToolConfig::getGlobal()->addAction(node, toType, "change_var_basetype"); 
  } else {
    ToolConfig::getGlobal()->addAction(node, toType, "change_var_type");
  }
}

void TFTypeTransformer::addTransformation(SgNode * node, SgType * type, bool base) {
  assert(transformations.find(node) == transformations.end());

  if (base) {
    type = rebuildBaseType(::Typeforge::typechain.getType(node), type);
  }

  transformations[node] = type;
}

void TFTypeTransformer::execute() {
  for (auto i = transformations.begin(); i != transformations.end(); i++){
    SgNode * node = i->first;
    SgType * type = i->second;
    string  location = ::Typeforge::typechain.getHandle(node);
    if (SgInitializedName * iname = isSgInitializedName(node)) {
      TFTypeTransformer::trace("Execution: Changing variable type @"+location+" to type "+type->unparseToString());

      iname->set_type(type);

    } else if (SgVariableDeclaration * vdecl = isSgVariableDeclaration(node)) {
      TFTypeTransformer::trace("Execution: Changing variable type @"+location+" to type "+type->unparseToString());

      SgNodeHelper::getInitializedNameOfVariableDeclaration(vdecl)->set_type(type);

    } else if (SgFunctionDeclaration* funDecl = isSgFunctionDeclaration(node)) {
      TFTypeTransformer::trace("Execution: Changing return type @"+location+" to type "+type->unparseToString());

      SgFunctionType * old_ftype = funDecl->get_type();
      assert(old_ftype != NULL);

      SgFunctionType * ftype = SageBuilder::buildFunctionType(type, old_ftype->get_argument_list());
      assert(ftype != NULL);

      std::set<SgFunctionDeclaration *> fdecls; // FIXME That way of building this set is really inefficient
      fdecls.insert(funDecl);
      for (auto fdecl : SgNodeHelper::listOfFunctionDeclarations(nullptr)) {
        SgFunctionDeclaration * fd = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
        if (fd == funDecl) {
          fdecls.insert(fdecl);
        }
      }
      for (auto fdecl : fdecls) {
        fdecl->set_type(ftype);
      }

    } else {
      // TODO SgFunctionCallExp
      cerr << "Error: attempted to apply changes to an unknown node " << node->class_name() <<endl;
      std::abort();
    }
  }
  addExplicitCast();
}

///////////////////////////////////////////////////////////////////////////

//given an initialized name will change it's type to the new given type
int TFTypeTransformer::changeType(SgNode * node, SgType * type, bool base, bool listing) {

#if DEBUG__TFTypeTransformer__changeType
  std::cout << "ENTER TFTypeTransformer::changeType" << std::endl;
  std::cout << "  node = " << node << " (" << ( node != nullptr ? node->class_name() : "" ) << ")" << std::endl;
  std::cout << "  type = " << type << " (" << ( type != nullptr ? type->class_name() : "" ) << ")" << std::endl;
  std::cout << "  base = " << base << std::endl;
#endif

  if (listing) {
    addToActionList(node, type, base);
    return 0;
  } else {
    addTransformation(node, type, base);
    return 1;
  }
}

bool TFTypeTransformer::_traceFlag=false;

void TFTypeTransformer::setTraceFlag(bool traceFlag) {
  _traceFlag=traceFlag;
}

bool TFTypeTransformer::getTraceFlag() {
  return _traceFlag;
}

void TFTypeTransformer::trace(string s) {
  if(TFTypeTransformer::_traceFlag) {
    cout<<"TRACE: "<<s<<endl;
  }
}

void TFTypeTransformer::generateCsvTransformationStats(string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation) {
  stringstream ss;
  ss<<numTypeReplace
    <<","<<tt._totalNumChanges
    <<","<<tfTransformation.readTransformations
    <<","<<tfTransformation.writeTransformations
    <<","<<tfTransformation.arrayOfStructsTransformations
    <<","<<tfTransformation.adIntermediateTransformations
    <<endl;
  CppStdUtilities::writeFile(fileName,ss.str());
}

void TFTypeTransformer::printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation) {
  stringstream ss;
  int numReplacementsFound=tt._totalNumChanges;
  int arrayReadAccesses=tfTransformation.readTransformations;
  int arrayWriteAccesses=tfTransformation.writeTransformations;
  int arrayOfStructsAccesses=tfTransformation.arrayOfStructsTransformations;
  int adIntermediateTransformations=tfTransformation.adIntermediateTransformations;
  cout<<"STATS: number of variable type replacements: "<<numReplacementsFound<<endl;
  cout<<"STATS: number of transformed array read accesses: "<<arrayReadAccesses<<endl;
  cout<<"STATS: number of transformed array write accesses: "<<arrayWriteAccesses<<endl;
  cout<<"STATS: number of transformed arrays of structs accesses: "<<arrayOfStructsAccesses<<endl;
  cout<<"STATS: number of ad_intermediate transformations: "<<adIntermediateTransformations<<endl;
  int totalTransformations=numReplacementsFound+arrayReadAccesses+arrayWriteAccesses+arrayOfStructsAccesses+adIntermediateTransformations;
  cout<<"STATS: total number of transformations: "<<totalTransformations<<endl;
}

TFTypeTransformer transformer;

}

