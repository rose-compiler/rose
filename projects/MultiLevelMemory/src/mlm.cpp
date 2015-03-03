#include "mlmapi.h"
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

extern vector<SgStatement*> DeletepragmasList;
extern vector<SgCallExpression*> callExprList;
extern vector<SgForStatement*> forStmtList;

using namespace MLMAPIInsertion;

void mlmFrontend::attachAttribute(SgPragmaDeclaration* pragma, AstAttribute* attr)
{
// attribute to specify memory level
  if(isSgVariableDeclaration(getNextStatement(pragma)))
  {
    SgVariableDeclaration* decl = isSgVariableDeclaration(getNextStatement(pragma));  
    ROSE_ASSERT(decl);
    SgInitializedNamePtrList nameList = decl->get_variables();
    if(nameList.size() == 1)
    {
      SgInitializedName* initName = nameList[0];
      SgSymbol* symbol = initName->get_symbol_from_symbol_table();
      symbol->setAttribute("mlmAttribute", attr);
    } 
  }
// attribute to specify tiling level
  else if(isSgForStatement(getNextStatement(pragma)))
  {
    SgForStatement* forStmt = isSgForStatement(getNextStatement(pragma));
    ROSE_ASSERT(forStmt);
    forStmt->setAttribute("mlmAttribute", attr);
  }
}

void mlmFrontend::visit(SgNode* node)
{
  if (isSgPragmaDeclaration(node) != NULL)
  {
    SgPragmaDeclaration * pragDecl = isSgPragmaDeclaration(node);
    char* end;
    int type = strtol(pragDecl->get_pragma()->get_pragma().c_str(),&end, 10);
    if(end == pragDecl->get_pragma()->get_pragma().c_str())
    {
      cout << "pragma has no integer value" << endl;
      type = -1;
    } 
    if(type >= 0) 
    {
      cout << "type = " << type << endl;
      mlmAttribute* newAttr = new mlmAttribute(type);
      mlmFrontend::attachAttribute(pragDecl, newAttr);
      DeletepragmasList.push_back(pragDecl);
    }
    // handle omp parallel, omp for, omp single, ..etc.
    //createOmpAttributeOld(node);
   }

}


void mlmTransform::visit(SgNode* node)
{
  // Searching for malloc, memcpy, and free
  if (isSgCallExpression(node) != NULL)
  {
    SgCallExpression* callExp = isSgCallExpression(node);
    ROSE_ASSERT(callExp);
    callExprList.push_back(callExp);
  }
  else if(isSgForStatement(node))
  {
    SgForStatement* forStmt = isSgForStatement(node);
    ROSE_ASSERT(forStmt);
    forStmtList.push_back(forStmt);
  }
}

void mlmTransform::transformForStmt(SgForStatement* forStmt)
{
    ROSE_ASSERT(forStmt);
    AstAttribute* attr = forStmt->getAttribute("mlmAttribute");
    if(attr)
    {
      mlmAttribute* mlmAttr = dynamic_cast<mlmAttribute*> (attr);
      int tileArg = mlmAttr->getMemType();
//      bool result=false;
//      result = loopTiling(forStmt,2, tileArg);
//      ROSE_ASSERT(result != false);
/*
      FILE* tileFile;
      tileFile = fopen("tile.sizes", "w");
      fprintf(tileFile, "%d %d %d",tileArg,tileArg, 1);
      fclose(tileFile);
      int polyargc = 4;
      char* polyargv[polyargc];
      polyargv[0] = "";
      polyargv[1] = "--polyopt-fixed-tiling";
      polyargv[2] = "--polyopt-scop-extractor-verbose=4";
      polyargv[4] = "--polyopt-verbose";
      PolyRoseOptions polyoptions (polyargc, polyargv);
      int retval;
      retval = PolyOptOptimizeSubTree(forStmt, polyoptions);
*/
      int retval;
      retval = PolyOptLoopTiling(forStmt,tileArg,tileArg,1); 
    }
}


void mlmTransform::transformCallExp(SgCallExpression* callExp)
{
    ROSE_ASSERT(callExp);
    SgFunctionRefExp* funcName = isSgFunctionRefExp(callExp->get_function());
    if(!funcName) return;
    SgExprListExp* funcArgList = callExp->get_args();
    SgExpressionPtrList argList = funcArgList->get_expressions();
    SgScopeStatement* scope = getScope(callExp);         
    cout << funcName->get_symbol()->get_name() << endl;

    /** if it is malloc, search for the mlm attribute and append the memory level **/
    if(strncmp("malloc",funcName->get_symbol()->get_name().str(),6) == 0)
    {
      if(argList.size() != 1)  return;
      SgExprListExp* funcArgList = callExp->get_args();

      // check if LHS of malloc has an attribute assigned
      SgNode* parentNode = callExp->get_parent();
      // parent node can be a casting expression
      while(isSgCastExp(parentNode))
      {
        parentNode = parentNode->get_parent();
      }
      cout << parentNode->class_name() << endl;
      // the mlm attribute
      AstAttribute* attr = NULL;
      // So far we spot two candidates for parentNode that we need to transform
      if(isSgAssignOp(parentNode))
      {
        SgAssignOp* assignOp = isSgAssignOp(parentNode);
        SgExpression* lhs = isSgExpression(assignOp->get_lhs_operand());
        if(!isSgVarRefExp(lhs))
        {
          cout << "lhs:" << assignOp->get_lhs_operand()->class_name() << endl;

          // if pointer is packaged inside a struct, then we need to look down in lhs.
          if(isSgDotExp(lhs))
          {
            lhs = isSgDotExp(lhs)->get_rhs_operand();
          }
        }
        SgVarRefExp* lhsVarRef = isSgVarRefExp(lhs);
        ROSE_ASSERT(lhsVarRef);
        SgSymbol* symbol = lhsVarRef->get_symbol();
        ROSE_ASSERT(symbol);
        //retrieve the attribute from symbol
        attr = symbol->getAttribute("mlmAttribute");
        cout << "LHS symbol name: " << symbol->get_name() << endl;
      }
      else if(isSgAssignInitializer(parentNode))
      {
        SgInitializedName* initName = isSgInitializedName(parentNode->get_parent());
        ROSE_ASSERT(initName);
        SgSymbol* symbol = initName->get_symbol_from_symbol_table();
        if(!symbol) return;
        ROSE_ASSERT(symbol);
        //retrieve the attribute from symbol
        attr = symbol->getAttribute("mlmAttribute");
        cout << "Initialized symbol name: " << symbol->get_name() << endl;
      }
      else
      {
        // do nothing because no attribute assigned or we always set to default
      }
      // if there is a mlm attribute attached to the symbol, then create new malloc
      if(attr)
      {
        mlmAttribute* mlmAttr = dynamic_cast<mlmAttribute*> (attr);
        SgExprListExp* funcArgList = callExp->get_args();
        funcArgList->append_expression(buildIntVal(mlmAttr->getMemType()));
        replaceExpression(callExp, buildFunctionCallExp("mlm_malloc",deepCopy(callExp->get_type()),deepCopy(funcArgList),getScope(callExp)));
      }
    }
    else if(strncmp("memcpy",funcName->get_symbol()->get_name().str(),6) == 0)
    {
//      cout << "replacing memcpy" << endl;
      if(argList.size() != 3)  return;
      Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(funcArgList, V_SgVarRefExp);
      SgVarRefExp* dst = isSgVarRefExp(varList[0]);
      SgVarRefExp* src = isSgVarRefExp(varList[1]);
      AstAttribute* attrDst = dst->get_symbol()->getAttribute("mlmAttribute");
      AstAttribute* attrSrc = src->get_symbol()->getAttribute("mlmAttribute");
      mlmAttribute* mlmAttrDst =  dynamic_cast<mlmAttribute*>(attrDst);
      mlmAttribute* mlmAttrSrc =  dynamic_cast<mlmAttribute*>(attrSrc);
//      if((mlmAttrDst && !mlmAttrSrc) || (mlmAttrDst && mlmAttrSrc && (mlmAttrDst->getMemType() < mlmAttrDst->getMemType())))
//      {
//        replaceExpression(callExp, buildFunctionCallExp("mlm_memcpy",deepCopy(callExp->get_type()),deepCopy(funcArgList),scope),true);
//        DeletepragmasList2.push_back(callExp);
//      }
//
//      else if((!mlmAttrDst && mlmAttrSrc) || (mlmAttrDst && mlmAttrSrc && (mlmAttrDst->getMemType() > mlmAttrDst->getMemType())))

// 09/30/14 Following Simon's suggestion, we always insert wait for the mlm_memcpy
//      {
         string tagName = generateUniqueVariableName(scope,"copy_tag");
         SgVariableDeclaration* newDecl = buildVariableDeclaration(tagName,
                                          buildOpaqueType("mlm_Tag",getGlobalScope(callExp)), 
                                          buildAssignInitializer(buildFunctionCallExp("mlm_memcpy",deepCopy(callExp->get_type()),deepCopy(funcArgList),scope)));
         SgExprStatement* waitStmt = buildFunctionCallStmt("mlm_waitComplete",
                                                           buildVoidType(),
                                                           buildExprListExp(buildVarRefExp(tagName,scope)),
                                                           scope); 
         insertStatement(getEnclosingStatement(callExp),newDecl,true);
         insertStatement(getEnclosingStatement(callExp),waitStmt,true);
         removeStatement(getEnclosingStatement(callExp));
//      }
    }
    else if(strncmp("free",funcName->get_symbol()->get_name().str(),4) == 0)
    {
//      cout << "replacing free" << endl;
      if(argList.size() != 1)  return;
      SgExpression* varExp = isSgExpression(argList[0]);
//cout << "exp:" << varExp->class_name() << endl;
      if(!isSgVarRefExp(varExp))
      {
        if(isSgCastExp(varExp))
        {
          varExp = isSgCastExp(varExp)->get_operand_i();
        }
        // if pointer is packaged inside a struct, then we need to look down in lhs.
        if(isSgDotExp(varExp))
        {
          varExp = isSgDotExp(varExp)->get_rhs_operand();
        }
      }
      SgVarRefExp* varRef = isSgVarRefExp(varExp);
      ROSE_ASSERT(varRef);
      AstAttribute* attr = varRef->get_symbol()->getAttribute("mlmAttribute");
      if(attr)
      {
        replaceExpression(callExp, buildFunctionCallExp("mlm_free",deepCopy(callExp->get_type()),deepCopy(funcArgList),scope),false);
      }
    }

}

void mlmTransform::insertHeaders(SgProject* project)
{
  Rose_STL_Container<SgNode*> globalList = NodeQuery::querySubTree (project,V_SgGlobal);
  for(Rose_STL_Container<SgNode*>::iterator i = globalList.begin(); i != globalList.end(); i++)
  {
    SgGlobal* global = isSgGlobal(*i);
    ROSE_ASSERT(global);

    PreprocessingInfo* headerInfo = insertHeader("mlm.h",PreprocessingInfo::after,false,global);
    headerInfo->set_file_info(global->get_file_info());
  }
  return; //assume always successful currently
}

bool mlmTransform::loopTiling(SgForStatement* loopNest, size_t tileDims, size_t tileSize)
{
  ROSE_ASSERT(loopNest != NULL);
  ROSE_ASSERT(tileDims >0);
 // ROSE_ASSERT(tileSize>0);// 1 is allowed
 // skip tiling if tiling size is 0 (no tiling), we allow 0 to get a reference value for the original code being tuned
 // 1 (no need to tile)
  if (tileSize<=1)
    return true;
  // Locate the target loop at level n
  std::vector<SgForStatement* > loops= SageInterface::querySubTree<SgForStatement>(loopNest,V_SgForStatement);
  ROSE_ASSERT(loops.size()>=tileDims);
  for(int id = 0; id < tileDims; id++)
  {
  SgForStatement* target_loop = loops[id]; // adjust to numbering starting from 0

  // normalize the target loop first
  if (!forLoopNormalization(target_loop));
  {// the return value is not reliable
//    cerr<<"Error in SageInterface::loopTiling(): target loop cannot be normalized."<<endl;
//    dumpInfo(target_loop);
//    return false;
  }
   // grab the target loop's essential header information
   SgInitializedName* ivar = NULL;
   SgExpression* lb = NULL;
   SgExpression* ub = NULL;
   SgExpression* step = NULL;
   if (!isCanonicalForLoop(target_loop, &ivar, &lb, &ub, &step, NULL))
   {
     cerr<<"Error in SageInterface::loopTiling(): target loop is not canonical."<<endl;
     dumpInfo(target_loop);
     return false;
   }
   ROSE_ASSERT(ivar&& lb && ub && step);

  // Add a controlling loop around the top loop nest
  // Ensure the parent can hold more than one children
  SgLocatedNode* parent = NULL; //SageInterface::ensureBasicBlockAsParent(loopNest)
  if (isBodyStatement(loopNest)) // if it is a single body statement (Already a for statement, not a basic block)
   parent = makeSingleStatementBodyToBlock (loopNest);
  else
    parent = isSgLocatedNode(loopNest ->get_parent());

  ROSE_ASSERT(parent!= NULL);
     // Now we can prepend a controlling loop index variable: __lt_var_originalIndex
  string ivar2_name = "_lt_var_"+ivar->get_name().getString();
  SgScopeStatement* scope = loopNest->get_scope();
  SgVariableDeclaration* loop_index_decl = buildVariableDeclaration
  (ivar2_name, buildIntType(),NULL, scope);
  insertStatementBefore(loopNest, loop_index_decl);
   // init statement of the loop header, copy the lower bound
   SgStatement* init_stmt = buildAssignStatement(buildVarRefExp(ivar2_name,scope), copyExpression(lb));
   //two cases <= or >= for a normalized loop
   SgExprStatement* cond_stmt = NULL;
   SgExpression* orig_test = target_loop->get_test_expr();
   if (isSgBinaryOp(orig_test))
   {
     if (isSgLessOrEqualOp(orig_test))
       cond_stmt = buildExprStatement(buildLessOrEqualOp(buildVarRefExp(ivar2_name,scope),copyExpression(ub)));
     else if (isSgGreaterOrEqualOp(orig_test))
     {
       cond_stmt = buildExprStatement(buildGreaterOrEqualOp(buildVarRefExp(ivar2_name,scope),copyExpression(ub)));
       }
     else
     {
       cerr<<"Error: illegal condition operator for a canonical loop"<<endl;
       dumpInfo(orig_test);
       ROSE_ASSERT(false);
     }
   }
   else
   {
     cerr<<"Error: illegal condition expression for a canonical loop"<<endl;
     dumpInfo(orig_test);
     ROSE_ASSERT(false);
   }
   ROSE_ASSERT(cond_stmt != NULL);

   // build loop incremental  I
   // expression var+=up*tilesize or var-=upper * tilesize
   SgExpression* incr_exp = NULL;
   SgExpression* orig_incr_exp = target_loop->get_increment();
   if( isSgPlusAssignOp(orig_incr_exp))
   {
     incr_exp = buildPlusAssignOp(buildVarRefExp(ivar2_name,scope), buildMultiplyOp(copyExpression(step), buildIntVal(tileSize)));
   }
    else if (isSgMinusAssignOp(orig_incr_exp))
    {
      incr_exp = buildMinusAssignOp(buildVarRefExp(ivar2_name,scope), buildMultiplyOp(copyExpression(step), buildIntVal(tileSize)));
    }
    else
    {
      cerr<<"Error: illegal increment expression for a canonical loop"<<endl;
      dumpInfo(orig_incr_exp);
      ROSE_ASSERT(false);
    }
    SgForStatement* control_loop = buildForStatement(init_stmt, cond_stmt,incr_exp, buildBasicBlock());
  insertStatementBefore(loopNest, control_loop);
  // move loopNest into the control loop
  removeStatement(loopNest);
  appendStatement(loopNest,isSgBasicBlock(control_loop->get_loop_body()));

  if(id == tileDims-1)
  {
    map<SgVariableSymbol*, SgPntrArrRefExp*> variableMap;
    std::vector<SgPntrArrRefExp* > pntrs= SageInterface::querySubTree<SgPntrArrRefExp>(loopNest,V_SgPntrArrRefExp);
    for(std::vector<SgPntrArrRefExp* >::iterator it = pntrs.begin(); it != pntrs.end(); ++ it)
    {
      vector<SgVariableSymbol*> tmp = SageInterface::getSymbolsUsedInExpression(*it);
      for(vector<SgVariableSymbol*>::iterator vit = tmp.begin(); vit != tmp.end(); ++vit)
      {
        //cout << "name = " << (*vit)->get_name()<< " type=" << (*vit)->get_type()->class_name() << endl;
        if(isSgArrayType((*vit)->get_type()) || isSgPointerType((*vit)->get_type()))
          variableMap[*vit] = *it;
      }
    }

    for(map<SgVariableSymbol*, SgPntrArrRefExp*>::iterator it=variableMap.begin(); it != variableMap.end(); ++it)
    {
  	string subArrayName = (*it).first->get_name().getString()+"_sub";
        cout << "name = " << (*it).first->get_name() << " new name:" << subArrayName << endl;
        SgType* subType = (*it).first->get_type();
        SgExpression* sizeExp;
        SgArrayType* oldtype;
        SgType* baseType;
        if(isSgArrayType((*it).first->get_type()))
        {
          // create the mutlti-dim data type for the subdomain
          oldtype = isSgArrayType((*it).first->get_type());
          std::vector<SgExpression*> arrayInfo = SageInterface::get_C_array_dimensions(oldtype);
          baseType = getArrayElementType(oldtype);
//cout << "dim size= " << getDimensionCount(oldtype) << " base type=" << baseType->class_name() << endl;
          for(int i=0; i <getDimensionCount(oldtype);++i)
          {
            SgArrayType* tmpArrayType;
            if(i == 0)
            {
              tmpArrayType = buildArrayType(baseType,buildIntVal(tileSize));
              sizeExp = buildIntVal(tileSize);
            }
            else
            {
              tmpArrayType = buildArrayType(baseType,buildIntVal(tileSize));
              sizeExp = buildMultiplyOp(sizeExp, buildIntVal(tileSize));
            }
            baseType = isSgType(tmpArrayType);
          }

          subType = buildPointerType(baseType);
        }
        // Building variable Declaration with malloc as initializer
        sizeExp = buildMultiplyOp(buildSizeOfOp(getArrayElementType(oldtype)),sizeExp);
        SgExprListExp* funcCallArgs = buildExprListExp(sizeExp);
        SgAssignInitializer* assignInit = buildAssignInitializer(buildFunctionCallExp("malloc",subType,funcCallArgs,scope));
  	SgVariableDeclaration* subArrayDecl = buildVariableDeclaration(subArrayName, subType,assignInit, scope);
        insertStatementBefore(loopNest, subArrayDecl);

        //Building memcpy function call
        //The offset, and size to be copied is still under development
        SgExprListExp* memcpyArgs = buildExprListExp(buildAddressOfOp(buildVarRefExp(subArrayName,scope)), buildAddressOfOp(buildVarRefExp((*it).first->get_name(),scope)), buildSizeOfOp(getArrayElementType(oldtype)));
        SgExprStatement* memcpyStmt = buildFunctionCallStmt("memcpy",buildVoidType(),memcpyArgs ,scope);
        insertStatementBefore(loopNest, memcpyStmt);
        
        SgPntrArrRefExp* pntrArrRef = (*it).second;
        pntrArrRef->set_lhs_operand(buildVarRefExp(subArrayName,scope)); 
    }
  } 

  // rewrite the lower (i=lb), upper bounds (i<=/>= ub) of the target loop
//  SgAssignOp* assign_op  = isSgAssignOp(lb->get_parent());
//  ROSE_ASSERT(assign_op);
//  assign_op->set_rhs_operand(buildVarRefExp(ivar2_name,scope));
    // ub< var_i+tileSize-1? ub:var_i+tileSize-1
  SgBinaryOp* bin_op = isSgBinaryOp(ub->get_parent());
  ROSE_ASSERT(bin_op);
//  SgExpression* ub2 = buildSubtractOp(buildAddOp(buildVarRefExp(ivar2_name,scope), buildIntVal(tileSize)), buildIntVal(1));
  SgExpression* ub2 = buildIntVal(tileSize);
//  SgExpression* test_exp = buildLessThanOp(copyExpression(ub),ub2);
//  test_exp->set_need_paren(true);
//  ub->set_need_paren(true);
//  ub2->set_need_paren(true);
//  SgConditionalExp * triple_exp = buildConditionalExp(test_exp,copyExpression(ub), copyExpression(ub2));
//  bin_op->set_rhs_operand(triple_exp);
  bin_op->set_rhs_operand(ub2);
  // constant folding
  // folding entire loop may decrease the accuracy of floating point calculation
  // we fold loop control expressions only
  //constantFolding(control_loop->get_scope());
  constantFolding(control_loop->get_test());
  constantFolding(control_loop->get_increment());
  }
  return true;
}
