#include "sage3basic.h"
#include "Specialization.h"

#include <map>

using namespace std;
using namespace SPRAY;

ConstReporter::~ConstReporter() {
}

SpecializationConstReporter::SpecializationConstReporter(VariableIdMapping* variableIdMapping, VariableId var, int constInt) {
    _variableIdMapping=variableIdMapping;
    _variableId=var;
    _constInt=constInt;
  }

VariableId SpecializationConstReporter::getVariableId() {
  return _variableId;
}

bool SpecializationConstReporter::isConst(SgNode* node) {
  if(SgVarRefExp* varRefExp=isSgVarRefExp(node)) {
    if(_variableIdMapping->variableId(varRefExp)==getVariableId()) {
      _varRefExp=varRefExp;
      return true;
    }
  }
  return false;
}

SgVarRefExp* SpecializationConstReporter::getVarRefExp() {
  return _varRefExp;
}
int SpecializationConstReporter::getConstInt() {
  return _constInt;
}


PStateConstReporter::PStateConstReporter(const PState* pstate, VariableIdMapping* variableIdMapping) {
    _pstate=pstate;
    _variableIdMapping=variableIdMapping;
  }

VariableId PStateConstReporter::getVariableId() {
  return _variableIdMapping->variableId(_varRefExp);
}

bool PStateConstReporter::isConst(SgNode* node) {
  if(SgVarRefExp* varRefExp=isSgVarRefExp(node)) {
    _varRefExp=varRefExp;
    VariableId varRefId=getVariableId();
    return _pstate->varIsConst(varRefId);
  }
  return false;
}

SgVarRefExp* PStateConstReporter::getVarRefExp() {
  return _varRefExp;
}
int PStateConstReporter::getConstInt() {
  VariableId varRefId=_variableIdMapping->variableId(_varRefExp);
  AValue varVal=_pstate->varValue(varRefId);
  ROSE_ASSERT(varVal.isConstInt());
  int varIntValue=varVal.getIntValue();
  return varIntValue;
}

int Specialization::specializeFunction(SgProject* project, string funNameToFind, int param, int constInt, VariableIdMapping* variableIdMapping) {
  std::list<SgFunctionDefinition*> funDefList=SgNodeHelper::listOfFunctionDefinitions(project);
  for(std::list<SgFunctionDefinition*>::iterator i=funDefList.begin();i!=funDefList.end();++i) {
    std::string funName=SgNodeHelper::getFunctionName(*i);
    if(funNameToFind==funName) {
      _specializedFunctionRootNode=*i;
      VariableId varId=determineVariableIdToSpecialize(*i,param,variableIdMapping);
      return substituteVariablesWithConst(*i, variableIdMapping, varId, constInt);
    }
  }
  return 0;
}

VariableId Specialization::determineVariableIdToSpecialize(SgFunctionDefinition* funDef, int param, VariableIdMapping* variableIdMapping) {
  VariableId variableId;
  SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
  int paramCnt=0;
  for(SgInitializedNamePtrList::iterator i=initNamePtrList.begin();i!=initNamePtrList.end();++i) {
    if(paramCnt==param) {
      SgInitializedName* initName=*i;
      return variableIdMapping->variableId(initName);
    }
    else
      paramCnt++;
  }

  // TODO
  return variableId;
}

int Specialization::substituteConstArrayIndexExprsWithConst(VariableIdMapping* variableIdMapping, ExprAnalyzer* exprAnalyzer, const EState* estate, SgNode* root) {
  typedef pair<SgExpression*,int> SubstitutionPair;
  typedef list<SubstitutionPair > SubstitutionList;
  SubstitutionList substitutionList;
  AstMatching m;
  MatchResult res;
  int numConstExprElim=0;
#pragma omp critical(EXPRSUBSTITUTION)
  {
    res=m.performMatching("SgPntrArrRefExp(_,$ArrayIndexExpr)",root);
  }
  if(res.size()>0) {
     for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
         // match found
       SgExpression* arrayIndexExpr=isSgExpression((*i)["$ArrayIndexExpr"]);
       if(arrayIndexExpr) {
         // avoid substituting a constant by a constant
         if(!isSgIntVal(arrayIndexExpr)) {
           list<SingleEvalResultConstInt> evalResultList=exprAnalyzer->evalConstInt(arrayIndexExpr,*estate,true, true);
           // only when we get exactly one result it is considered for substitution
           // there can be multiple const-results which do not allow to replace it with a single const
           if(evalResultList.size()==1) {
             list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
             ROSE_ASSERT(evalResultList.size()==1);
             AValue varVal=(*i).value();
             if(varVal.isConstInt()) {
               int varIntValue=varVal.getIntValue();
               //cout<<"INFO: const: "<<varIntValue<<" substituting: "<<arrayIndexExpr->unparseToString()<<endl;
               SgNodeHelper::replaceExpression(arrayIndexExpr,SageBuilder::buildIntVal(varIntValue),false);
               numConstExprElim++;
             }
           }
         }
       }
     }
   }
   return numConstExprElim;
 }

int Specialization::substituteVariablesWithConst(VariableIdMapping* variableIdMapping, const PState* pstate, SgNode *node) {
  ConstReporter* constReporter=new PStateConstReporter(pstate,variableIdMapping);
  int numOfSubstitutions=substituteVariablesWithConst(node, constReporter);
  delete constReporter;
  return numOfSubstitutions;
}

int Specialization::substituteVariablesWithConst(SgNode* node, VariableIdMapping* variableIdMapping, VariableId variableId, int constInt) {
  ConstReporter* constReporter=new SpecializationConstReporter(variableIdMapping,variableId,constInt);
  int numOfSubstitutions=substituteVariablesWithConst(node, constReporter);
  delete constReporter;
  return numOfSubstitutions;
}

int Specialization::substituteVariablesWithConst(SgNode* node, ConstReporter* constReporter) {
   typedef pair<SgExpression*,int> SubstitutionPair;
   typedef list<SubstitutionPair > SubstitutionList;
   SubstitutionList substitutionList;
   RoseAst ast(node);
   for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
     if(constReporter->isConst(*i)) {
       int varIntValue=constReporter->getConstInt();
       SgVarRefExp* varRefExp=constReporter->getVarRefExp();
       SubstitutionPair p=make_pair(varRefExp,varIntValue);
       substitutionList.push_back(p);
     }
   }
   for(SubstitutionList::iterator i=substitutionList.begin(); i!=substitutionList.end(); ++i) {
     // buildSignedIntType()
     // buildFloatType()
     // buildDoubleType()
     // SgIntVal* buildIntVal(int)
     SgNodeHelper::replaceExpression((*i).first,SageBuilder::buildIntVal((*i).second),false);
   }
   return (int)substitutionList.size();
 }


 bool Specialization::isAtMarker(Label lab, const EState* estate) {
   Label elab=estate->label();
   return elab==lab;
 }

void Specialization::extractArrayUpdateOperations(Analyzer* ana,
                                                  ArrayUpdatesSequence& arrayUpdates,
                                                  RewriteSystem& rewriteSystem,
                                                  bool useConstExprSubstRule
                                                  ) {
   Labeler* labeler=ana->getLabeler();
   VariableIdMapping* variableIdMapping=ana->getVariableIdMapping();
   TransitionGraph* tg=ana->getTransitionGraph();
   const EState* estate=tg->getStartEState();
   EStatePtrSet succSet=tg->succ(estate);
   ExprAnalyzer* exprAnalyzer=ana->getExprAnalyzer();
   int numProcessedArrayUpdates=0;
   vector<pair<const EState*, SgExpression*> > stgArrayUpdateSequence;

   while(succSet.size()>=1) {
     // investigate state
     Label lab=estate->label();
     SgNode* node=labeler->getNode(lab);
     // eliminate superfluous root nodes
     if(isSgExprStatement(node))
       node=SgNodeHelper::getExprStmtChild(node);
     if(isSgExpressionRoot(node))
       node=SgNodeHelper::getExprRootChild(node);
     if(SgExpression* exp=isSgExpression(node)) {
       if(SgNodeHelper::isArrayElementAssignment(exp)||SgNodeHelper::isFloatingPointAssignment(node)) {
         stgArrayUpdateSequence.push_back(make_pair(estate,exp));
       }
     }
     if(succSet.size()>1) {
       cerr<<estate->toString()<<endl;
       cerr<<"Error: STG-States with more than one successor not supported in term extraction yet."<<endl;
       exit(1);
     } else {
       EStatePtrSet::iterator i=succSet.begin();
       estate=*i;
     }  
     // next successor set
     succSet=tg->succ(estate);
   }

   // stgArrayUpdateSequence is now a vector of all array update operations from the STG
   // prepare array for parallel assignments of rewritten ASTs
   arrayUpdates.resize(stgArrayUpdateSequence.size());
   int N=stgArrayUpdateSequence.size();

   // this loop is prepared for parallel execution (but rewriting the AST in parallel causes problems)
   //  #pragma omp parallel for
   for(int i=0;i<N;++i) {
     const EState* p_estate=stgArrayUpdateSequence[i].first;
     const PState* p_pstate=p_estate->pstate();
     SgExpression* p_exp=stgArrayUpdateSequence[i].second;
     SgNode* p_expCopy;
     p_expCopy=SageInterface::copyExpression(p_exp);
#if 1
     // p_expCopy is a pointer to an assignment expression (only rewriteAst changes this variable)
     if(useConstExprSubstRule) {
       int numConstExprElim=substituteConstArrayIndexExprsWithConst(variableIdMapping, exprAnalyzer,p_estate,p_expCopy);
       rewriteSystem.getRewriteStatisticsPtr()->numConstExprElim+=numConstExprElim;
       rewriteSystem.rewriteCompoundAssignments(p_expCopy,variableIdMapping);
     } else {
       rewriteSystem.getRewriteStatisticsPtr()->numVariableElim+=substituteVariablesWithConst(variableIdMapping,p_pstate,p_expCopy);
       rewriteSystem.rewriteAst(p_expCopy, variableIdMapping);
     }
#endif
     SgExpression* p_expCopy2=isSgExpression(p_expCopy);
     if(!p_expCopy2) {
       cerr<<"Error: wrong node type in array update extraction. Expected SgExpression* but found "<<p_expCopy->class_name()<<endl;
       exit(1);
    }
    numProcessedArrayUpdates++;
    if(numProcessedArrayUpdates%100==0) {
      cout<<"INFO: transformed arrayUpdates: "<<numProcessedArrayUpdates<<" / "<<stgArrayUpdateSequence.size() <<endl;
    }
    rewriteSystem.getRewriteStatisticsPtr()->numArrayUpdates++;
    arrayUpdates[i]=EStateExprInfo(p_estate,p_expCopy2);
  }    
}


// searches the arrayUpdates vector backwards starting at pos, matches lhs array refs and returns a pointer to it (if not available it returns 0)
SgNode* Specialization::findDefAssignOfArrayElementUse(SgPntrArrRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping) {
  ArrayElementAccessData useRefData(useRefNode,variableIdMapping);
  do {
    SgPntrArrRefExp* lhs=isSgPntrArrRefExp(SgNodeHelper::getLhs((*pos).second));
    // there can be non-array element updates on lhs
    if(lhs) {
      ArrayElementAccessData defData(isSgPntrArrRefExp(lhs),variableIdMapping);
      if(defData==useRefData) {
        (*pos).mark=true; // mark each used definition
        return (*pos).second; // return pointer to assignment expression (instead directly to def);
      }
    }
    // there is no concept for before-the-start iterator (therefore this is checked this way) -> change this rbegin/rend
    if(pos==arrayUpdates.begin()) 
      break;
    --pos; 
  } while (1);

  return 0;
}

// searches the arrayUpdates vector backwards starting at pos, matches lhs array refs and returns a pointer to it (if not available it returns 0)
SgNode* Specialization::findDefAssignOfUse(SgVarRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping) {
  VariableId useRefId=variableIdMapping->variableId(useRefNode);
  do {
    SgVarRefExp* lhs=isSgVarRefExp(SgNodeHelper::getLhs((*pos).second));
    // there can be non-var-refs updates on lhs
    if(lhs) {
      VariableId defId=variableIdMapping->variableId(lhs);
      if(defId==useRefId) {
        (*pos).mark=true; // mark each used definition
        return (*pos).second; // return pointer to assignment expression (instead directly to def);
      }
    }
    // there is no concept for before-the-start iterator (therefore this is checked this way) -> change this rbegin/rend
    if(pos==arrayUpdates.begin()) 
      break;
    --pos; 
  } while (1);

  return 0;
}

// linear algorithm. Only works for a sequence of assignments.
void Specialization::createSsaNumbering(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping) {
  std::map<string,int> defVarNumbers;
  for(size_t i=0;i<arrayUpdates.size();++i) {
    SgExpression* exp=arrayUpdates[i].second;
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(exp));

    // determine SSA number of uses and attach
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(exp));
    ROSE_ASSERT(isSgPntrArrRefExp(lhs)||SgNodeHelper::isFloatingPointAssignment(exp));
    //cout<<"EXP: "<<exp->unparseToString()<<", lhs:"<<lhs->unparseToString()<<" :: "<<endl;
    RoseAst rhsast(rhs);
    for(RoseAst::iterator j=rhsast.begin();j!=rhsast.end();++j) {
      if(SgPntrArrRefExp* useRef=isSgPntrArrRefExp(*j)) {
        j.skipChildrenOnForward();
        ArrayElementAccessData access(useRef,variableIdMapping);
        string useName=access.toString(variableIdMapping);
        AstAttribute* attr=new NumberAstAttribute(defVarNumbers[useName]); // default creates 0 int (which is exactly what we need)
        if(attr) {
          useRef->setAttribute("Number",attr);
        }
      } // if array
      // this can be rewritten once it is clear that the element type of an array is properly reported by isFloatingPointExpr(exp)
      else if(SgVarRefExp* useRef=isSgVarRefExp(*j)) {
        ROSE_ASSERT(useRef);
        j.skipChildrenOnForward();
        VariableId varId=variableIdMapping->variableId(useRef);
        string useName=variableIdMapping->uniqueShortVariableName(varId);
        AstAttribute* attr=new NumberAstAttribute(defVarNumbers[useName]); // default creates 0 int (which is exactly what we need)
        if(attr) {
          useRef->setAttribute("Number",attr);
        }
      } else {
        //cout<<"INFO: UpdateExtraction: ignored expression on rhs:"<<(*j)->unparseToString()<<endl;
      }
    }

    // compute and attach SSA number for def (lhs)
    string name;
    SgNode* toAnnotate=0;
    if(SgPntrArrRefExp* arr=isSgPntrArrRefExp(lhs)) {
      ArrayElementAccessData access(arr,variableIdMapping);
      name=access.toString(variableIdMapping);
      toAnnotate=arr;
    } else if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
      VariableId varId=variableIdMapping->variableId(var);
      name=variableIdMapping->uniqueShortVariableName(varId);
      toAnnotate=var;
    } else {
      cerr<<"Error: SSA Numbering: unknown LHS."<<endl;
      exit(1);
    }
    if(toAnnotate) {
      if(defVarNumbers.count(name)==0) {
        defVarNumbers[name]=1;
      } else {
        defVarNumbers[name]=defVarNumbers[name]+1;
      }
      toAnnotate->setAttribute("Number",new NumberAstAttribute(defVarNumbers[name]));
    }
  } // end assignments for loop
}


// this function has become superfluous for SSA numbering (but for substituting uses with rhs of defs it is still necessary (1/2)
void Specialization::attachSsaNumberingtoDefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping) {
  std::map<string,int> defVarNumbers;
  for(size_t i=0;i<arrayUpdates.size();++i) {
    SgExpression* exp=arrayUpdates[i].second;
    SgNode* lhs=SgNodeHelper::getLhs(exp);
    string name;
    SgNode* toAnnotate=0;
    if(SgPntrArrRefExp* arr=isSgPntrArrRefExp(lhs)) {
      ArrayElementAccessData access(arr,variableIdMapping);
      name=access.toString(variableIdMapping);
      toAnnotate=arr;
    } else if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
      VariableId varId=variableIdMapping->variableId(var);
      name=variableIdMapping->uniqueShortVariableName(varId);
      toAnnotate=var;
    } else {
      cerr<<"Error: SSA Numbering: unknown LHS."<<endl;
      exit(1);
    }
    if(toAnnotate) {
      if(defVarNumbers.count(name)==0) {
        defVarNumbers[name]=1;
      } else {
        defVarNumbers[name]=defVarNumbers[name]+1;
      }
      toAnnotate->setAttribute("Number",new NumberAstAttribute(defVarNumbers[name]));
    }
  }
}

// this function has become superfluous for SSA numbering (but for substituting uses with rhs of defs it is still necessary (2/2)
void Specialization::substituteArrayRefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping, SAR_MODE sarMode) {
  ArrayUpdatesSequence::iterator i=arrayUpdates.begin();
  ++i; // we start at element 2 because no substitutions can be performed in the first one AND this simplifies passing the previous element (i-1) when starting the backward search
  for(;i!=arrayUpdates.end();++i) {
    SgExpression* exp=(*i).second;
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(exp));
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(exp));
    ROSE_ASSERT(isSgPntrArrRefExp(lhs)||SgNodeHelper::isFloatingPointAssignment(exp));
    //cout<<"EXP: "<<exp->unparseToString()<<", lhs:"<<lhs->unparseToString()<<" :: "<<endl;
    RoseAst rhsast(rhs);
    for(RoseAst::iterator j=rhsast.begin();j!=rhsast.end();++j) {
      if(SgPntrArrRefExp* useRef=isSgPntrArrRefExp(*j)) {
        j.skipChildrenOnForward();
        // search for def here
        ArrayUpdatesSequence::iterator i_copy=i;
        --i_copy; // necessary and guaranteed to not decrement i=begin() because the loop starts at (i=begin())++
        SgNode* defAssign=findDefAssignOfArrayElementUse(useRef, arrayUpdates, i_copy, variableIdMapping);
        if(defAssign) {
          SgExpression* defRhs=isSgExpression(SgNodeHelper::getRhs(defAssign));
          ROSE_ASSERT(defRhs);
          //cout<<"INFO: USE:"<<useRef->unparseToString()<< " DEF:"<<defAssign->unparseToString()<<"DEF-RHS"<<defRhs->unparseToString()<<endl;
          switch(sarMode) {
          case SAR_SUBSTITUTE: {
            SgNodeHelper::replaceExpression(useRef,SageInterface::copyExpression(defRhs),true); // must be true (otherwise internal error)
            break;
          }
          case SAR_SSA: {
            AstAttribute* attr=SgNodeHelper::getLhs(defAssign)->getAttribute("Number");
            if(attr) {
              useRef->setAttribute("Number",attr);
            }
            break;
          }
          } // end switch
        }
      } // if array
      // this can be rewritten once it is clear that the element type of an array is properly reported by isFloatingPointExpr(exp)
      else if(SgVarRefExp* useRef=isSgVarRefExp(*j)) {
        ROSE_ASSERT(useRef);
        j.skipChildrenOnForward();
        // search for def here
        ArrayUpdatesSequence::iterator i_copy=i;
        --i_copy; // necessary and guaranteed to not decrement i=begin() because the loop starts at (i=begin())++
        SgNode* defAssign=findDefAssignOfUse(useRef, arrayUpdates, i_copy, variableIdMapping);
        if(defAssign) {
          SgExpression* defRhs=isSgExpression(SgNodeHelper::getRhs(defAssign));
          ROSE_ASSERT(defRhs);
          //cout<<"INFO: USE:"<<useRef->unparseToString()<< " DEF:"<<defAssign->unparseToString()<<"DEF-RHS"<<defRhs->unparseToString()<<endl;
          switch(sarMode) {
          case SAR_SUBSTITUTE: {
            SgNodeHelper::replaceExpression(useRef,SageInterface::copyExpression(defRhs),true); // must be true (otherwise internal error)
            break;
          }
          case SAR_SSA: {
            AstAttribute* attr=SgNodeHelper::getLhs(defAssign)->getAttribute("Number");
            if(attr) {
              useRef->setAttribute("Number",attr);
            }
            break;
          }
          } // end switch
        }
      } else {
        //cout<<"INFO: UpdateExtraction: ignored expression on rhs:"<<(*j)->unparseToString()<<endl;
      }
    }
  }
}

void Specialization::printUpdateInfos(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping) {
  int cnt=0;
  for(ArrayUpdatesSequence::iterator i=arrayUpdates.begin();i!=arrayUpdates.end();++i) {
    const EState* estate=(*i).first;
    const PState* pstate=estate->pstate();
    SgExpression* exp=(*i).second;
    cout<<"UPD"<<cnt<<":"<<pstate->toString(variableIdMapping)<<" : "<<exp->unparseToString()<<endl;
    ++cnt;
  }
}

string Specialization::iterVarsToString(IterationVariables iterationVars, VariableIdMapping* variableIdMapping) {
  stringstream ss;
  bool exists=false;
  for(IterationVariables::iterator i=iterationVars.begin();i!=iterationVars.end();++i) {
    if(i!=iterationVars.begin())
      ss<<", ";
    ss<<variableIdMapping->variableName((*i).first);
    if((*i).second) {
      ss<<"[par]";
      exists=true;
    }
  }
#if 0
  if(!exists) {
    cerr<<"Error: iterVarsAndParVarToString:: parallel iteration var is not an iteration variable."<<endl;
    cerr<<"Variable: "<<variableIdMapping->variableName(parallelIterationVar)<<endl;
    exit(1);
  }
#endif
  return ss.str();
}

int Specialization::verifyUpdateSequenceRaceConditions(IterationVariables iterationVars, ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping) {
  int cnt=0;
  stringstream ss;
  cout<<"STATUS: check race conditions: "<<iterVarsToString(iterationVars,variableIdMapping)<<endl;
  VariableId parVariable;
  for(IterationVariables::iterator i=iterationVars.begin();i!=iterationVars.end();++i) {
    if((*i).second==ITERVAR_PAR)
      parVariable=(*i).first;
  }
  // no parallel iter-var implies that the program is a sequential program. Therefore no race conditions exist.
  if(!parVariable.isValid()) {
    return 0;
  }

  // race check
  // union w-set_i = empty
  // union r-set_i intersect union w-set_j = empty, i!=j.
  // VariableIdSet computeWSet(VariableId,UpdateSequence)
  // VariableIdSet computeRSet(VariableId,UpdateSequence)
  // int<set> computeValues(VariableId,UpdateSequence)
  // xxxx
  ArrayElementAccessDataSet writeArrayAccessSet;
  VariableIdSet writeVarIdSet;
  ArrayElementAccessDataSet readArrayAccessSet;
  VariableIdSet readVarIdSet;
  for(ArrayUpdatesSequence::iterator i=arrayUpdates.begin();i!=arrayUpdates.end();++i) {
    const EState* estate=(*i).first;
    const PState* pstate=estate->pstate();
    SgExpression* exp=(*i).second;
    // intentionally not used (temporary placeholder)

    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(exp));
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(exp));
    ROSE_ASSERT(isSgPntrArrRefExp(lhs)||SgNodeHelper::isFloatingPointAssignment(exp));

    //cout<<"EXP: "<<exp->unparseToString()<<", lhs:"<<lhs->unparseToString()<<" :: "<<endl;
    // read-set
    RoseAst rhsast(rhs);
    for(RoseAst::iterator j=rhsast.begin();j!=rhsast.end();++j) {
      if(SgPntrArrRefExp* useRef=isSgPntrArrRefExp(*j)) {
        j.skipChildrenOnForward();
        ArrayElementAccessData access(useRef,variableIdMapping);
        readArrayAccessSet.insert(access);
      } // if array
      // this can be rewritten once it is clear that the element type of an array is properly reported by isFloatingPointExpr(exp)
      else if(SgVarRefExp* useRef=isSgVarRefExp(*j)) {
        ROSE_ASSERT(useRef);
        j.skipChildrenOnForward();
        VariableId varId=variableIdMapping->variableId(useRef);
        readVarIdSet.insert(varId);
      } else {
        //cout<<"INFO: UpdateExtraction: ignored expression on rhs:"<<(*j)->unparseToString()<<endl;
      }
    }
    if(SgPntrArrRefExp* arr=isSgPntrArrRefExp(lhs)) {
      ArrayElementAccessData access(arr,variableIdMapping);
      writeArrayAccessSet.insert(access);
    } else if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
      VariableId varId=variableIdMapping->variableId(var);
      writeVarIdSet.insert(varId);
    } else {
      cerr<<"Error: SSA Numbering: unknown LHS."<<endl;
      exit(1);
    }

    ss<<"UPD"<<cnt<<":"<<pstate->toString(variableIdMapping)<<" : "<<exp->unparseToString()<<endl;
    ++cnt;
  }
  cout<<"DEBUG: read-array-access:"<<readArrayAccessSet.size()<<" read-var-access:"<<readVarIdSet.size()<<endl;

  return 0;
}

void Specialization::writeArrayUpdatesToFile(ArrayUpdatesSequence& arrayUpdates, string filename, SAR_MODE sarMode, bool performSorting) {
  // 1) create vector of generated assignments (preparation for sorting)
  vector<string> assignments;
  for(ArrayUpdatesSequence::iterator i=arrayUpdates.begin();i!=arrayUpdates.end();++i) {
    switch(sarMode) {
    case SAR_SSA: {
      // annotate AST for unparsing Array-SSA form
      RoseAst ast((*i).second);
      for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
        if((*j)->attributeExists("Number")) {
          ROSE_ASSERT(isSgPntrArrRefExp(*j)||isSgVarRefExp(*j));
          if((*j)->attributeExists("AstUnparseAttribute"))
            (*j)->removeAttribute("AstUnparseAttribute");
          ROSE_ASSERT(!(*j)->attributeExists("AstUnparseAttribute"));
          AstUnparseAttribute* ssaNameAttribute=new AstUnparseAttribute((*j)->unparseToString()+string("_")+(*j)->getAttribute("Number")->toString(),AstUnparseAttribute::e_replace);
          (*j)->setAttribute("AstUnparseAttribute",ssaNameAttribute);
        }            
      }
      assignments.push_back((*i).second->unparseToString());
      break;
    }
    case SAR_SUBSTITUTE: {
      if(!(*i).mark)
        assignments.push_back((*i).second->unparseToString());
    }
    }
  }
  if(performSorting) {
    sort(assignments.begin(),assignments.end());
  }
  ofstream myfile;
  myfile.open(filename.c_str());
  for(vector<string>::iterator i=assignments.begin();i!=assignments.end();++i) {
    myfile<<(*i)<<endl;
  }
  myfile.close();
}

#if 0
SgExpressionPtrList& Specialization::getInitializerListOfArrayVariable(VariableId arrayVar, VariableIdMapping* variableIdMapping) {
  SgVariableDeclaration* decl=variableIdMapping->getVariableDeclaration(arrayVar);
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    // array initializer
    SgInitializer* initializer=initName->get_initializer();
    if(SgAggregateInitializer* arrayInit=isSgAggregateInitializer(initializer)) {
      SgExprListExp* rhsOfArrayInit=arrayInit->get_initializers();
      SgExpressionPtrList& exprPtrList=rhsOfArrayInit->get_expressions();
      return exprPtrList;
    }
  }
  cerr<<"Error: getInitializerListOfArrayVariable failed."<<endl;
  exit(1);
}
#endif    

string Specialization::flattenArrayInitializer(SgVariableDeclaration* decl, VariableIdMapping* variableIdMapping) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    // array initializer
    SgInitializer* initializer=initName->get_initializer();
    ROSE_ASSERT(initializer);
    stringstream ss;
    // x[2] = {1,2};"). In this case the SgExprListExp ("{1,2}") is wrapped in an SgAggregateInitializer
    // SgExprListExp* SgAggregateInitializer->get_initializers () const 
    // pointer variable initializer
    if(SgAggregateInitializer* arrayInit=isSgAggregateInitializer(initializer)) {
      string arrayName=variableIdMapping->variableName(variableIdMapping->variableId(decl));
      SgExprListExp* rhsOfArrayInit=arrayInit->get_initializers();
      //cout<<"RHS-ARRAY-INIT:"<<rhsOfArrayInit->unparseToString()<<endl;
      SgExpressionPtrList& exprPtrList=rhsOfArrayInit->get_expressions();
      string newRhs;
      int num=0;
      SgType* type=rhsOfArrayInit->get_type();
      for(SgExpressionPtrList::iterator i=exprPtrList.begin();i!=exprPtrList.end();++i) {
        ss<<type->unparseToString()<<" "<<arrayName<<"_"<<num<<" = "<<(*i)->unparseToString()<<";\n";
        num++;
      }
      string transformedArrayInitializer=ss.str();
      return transformedArrayInitializer;
    } else {
      cerr<<"Error: attempted to transform non-array initializer."<<endl;
      exit(1);
    }
  } else {
      cerr<<"Error: attempted to transform non-initialized declaration."<<endl;
      exit(1);
  }
}


void Specialization::transformArrayAccess(SgNode* node, VariableIdMapping* variableIdMapping) {
  ROSE_ASSERT(SgNodeHelper::isArrayAccess(node));
  if(SgPntrArrRefExp* arrayAccessAst=isSgPntrArrRefExp(node)) {
    ArrayElementAccessData arrayAccess(arrayAccessAst,variableIdMapping);
    ROSE_ASSERT(arrayAccess.getDimensions()==1);
    VariableId accessVar=arrayAccess.getVariable();
    int accessSubscript=arrayAccess.getSubscript(0);
    stringstream newAccess;
    newAccess<<variableIdMapping->variableName(accessVar)<<"_"<<accessSubscript;
    SgNodeHelper::replaceAstWithString(node,newAccess.str());
  } else {
    cerr<<"Error: transformation of array access failed."<<endl;
  }
}

void Specialization::transformArrayProgram(SgProject* root, Analyzer* analyzer) {
  // 1) transform initializers of global variables : a[]={1,2,3} ==> int a_0=1;int a_1=2;int a_2=3;
  // 2) eliminate initializers of pointer variables: int p* = a; ==> \eps
  // 3) replace uses of p[k]: with a_k (where k is a constant)

  //ad 1 and 2)
  VariableIdMapping* variableIdMapping=analyzer->getVariableIdMapping();
  Analyzer::VariableDeclarationList usedGlobalVariableDeclarationList=analyzer->computeUsedGlobalVariableDeclarationList(root);
  cout<<"STATUS: number of used global variables: "<<usedGlobalVariableDeclarationList.size()<<endl;
  list<pair<SgNode*,string> > toReplaceArrayInitializations;
  list<SgVariableDeclaration*> toDeleteDeclarations;
  typedef map<VariableId,VariableId> ArrayPointerMapType;
  ArrayPointerMapType arrayPointer; // var,arrayName
  for(Analyzer::VariableDeclarationList::iterator i=usedGlobalVariableDeclarationList.begin();
      i!=usedGlobalVariableDeclarationList.end();
      ++i) {
    SgVariableDeclaration* decl=*i;
    //cout<<"DEBUG: variableDeclaration:"<<decl->unparseToString()<<endl;
    SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
    ROSE_ASSERT(initName0);
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      // array initializer
      SgInitializer* arrayInitializer=initName->get_initializer();
      //string arrayName=variableIdMapping->variableName(variableIdMapping->variableId(*i));
      if(isSgAggregateInitializer(arrayInitializer)) {
        string transformedArrayInitializer=flattenArrayInitializer(decl,variableIdMapping);
        toReplaceArrayInitializations.push_back(make_pair(decl,transformedArrayInitializer));
        //SgNodeHelper::replaceAstWithString(decl,transformedArrayInitializer);
      } else {
        //cout<<"initName:"<<astTermWithNullValuesToString(initName)<<endl;
        VariableId lhsVarId=variableIdMapping->variableId(*i);
        string lhsVariableName=variableIdMapping->variableName(lhsVarId);
        SgType* type=variableIdMapping->getType(variableIdMapping->variableId(*i));
        // match: SgInitializedName(SgAssignInitializer(SgVarRefExp))
        // variable on lhs
        // check if variable is a pointer variable
        if(isSgPointerType(type)) {
          AstMatching m;
          MatchResult res;
          res=m.performMatching("SgInitializedName(SgAssignInitializer($varRef=SgVarRefExp))|SgInitializedName(SgAssignInitializer(SgAddressOfOp($varRef=SgVarRefExp)))",initName);
          if(res.size()==1) {
            toDeleteDeclarations.push_back(decl);
            MatchResult::iterator j=res.begin();
            SgVarRefExp* rhsVarRef=isSgVarRefExp((*j)["$varRef"]);
            VariableId rhsVarId=variableIdMapping->variableId(rhsVarRef);
            arrayPointer[lhsVarId]=rhsVarId;
            //cout<<"Inserted pair "<<variableName<<":"<<rhsVarName<<endl;
          }
        }
      }
    }
  }

  typedef list<pair<SgPntrArrRefExp*,ArrayElementAccessData> > ArrayAccessInfoType;
  ArrayAccessInfoType arrayAccesses;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgExpression* exp=isSgExpression(*i);
    if(exp) {
      if(SgPntrArrRefExp* arrAccess=isSgPntrArrRefExp(exp)) {
        ArrayElementAccessData aead(arrAccess,analyzer->getVariableIdMapping());
        ROSE_ASSERT(aead.isValid());
        //VariableId arrayVar=aead.varId;
        //cout<<"array-element: "<<variableIdMapping->variableName(arrayVar);
        if(aead.subscripts.size()==1) {
          //cout<<" ArrayIndex:"<<*aead.subscripts.begin();
          arrayAccesses.push_back(make_pair(arrAccess,aead));
        } else {
          cout<<"Error: ArrayIndex: unknown (dimension>1)";
          exit(1);
        }          
      }
    }
  }
#if 0
  cout<<"Array-Pointer Map:"<<endl;
  for(ArrayPointerMapType::iterator i=arrayPointer.begin();i!=arrayPointer.end();++i) {
    cout<<(*i).first.toString()<<":"<<(*i).second.toString()<<endl;
  }
#endif
  cout<<"STATUS: Replacing array-initializations."<<endl;
  for(list<pair<SgNode*,string> >::iterator i=toReplaceArrayInitializations.begin();i!=toReplaceArrayInitializations.end();++i) {
    //cout<<(*i).first->unparseToString()<<":\n"<<(*i).second<<endl;
    SgNodeHelper::replaceAstWithString((*i).first,"\n"+(*i).second);
  }
  cout<<"STATUS: Transforming pointer declarations."<<endl;
  for(list<SgVariableDeclaration*>::iterator i=toDeleteDeclarations.begin();i!=toDeleteDeclarations.end();++i) {
    //cout<<(*i)->unparseToString()<<endl;
    VariableId declaredPointerVar=variableIdMapping->variableId(*i);
    SgNode* initName0=(*i)->get_traversalSuccessorByIndex(1); // get-InitializedName
    ROSE_ASSERT(initName0);
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      // initializer
      SgInitializer* initializer=initName->get_initializer();
      if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
        //cout<<"var-initializer:"<<initializer->unparseToString()<<astTermWithNullValuesToString(initializer)<<endl;
        SgExpression* assignInitOperand=assignInitializer->get_operand_i();
        if(isSgAddressOfOp(assignInitOperand)) {
          assignInitOperand=isSgExpression(SgNodeHelper::getFirstChild(assignInitOperand));
          ROSE_ASSERT(assignInitOperand);
        }
        if(SgVarRefExp* rhsInitVar=isSgVarRefExp(assignInitOperand)) {
          VariableId arrayVar=variableIdMapping->variableId(rhsInitVar);
          SgExpressionPtrList& arrayInitializerList=variableIdMapping->getInitializerListOfArrayVariable(arrayVar);
          //cout<<"DEBUG: rhs array:"<<arrayInitializerList.size()<<" elements"<<endl;
          int num=0;
          stringstream ss;
          for(SgExpressionPtrList::iterator j=arrayInitializerList.begin();j!=arrayInitializerList.end();++j) {
            ss<<"int "<<variableIdMapping->variableName(declaredPointerVar)<<"_"<<num<<" = "
              <<(*j)->unparseToString()
              <<";\n"
              ;
            num++;
          }
          SgNodeHelper::replaceAstWithString(*i,"\n"+ss.str());
        }
      }
    }
#if 0
    ArrayElementAccessData arrayAccess=
    ROSE_ASSERT(arrayAccess.getDimensions()==1);
    VariableId accessVar=arrayAccess.getVariable();
    int accessSubscript=arrayAccess.getSubscript(0);
#endif
    //SageInterface::removeStatement(*i);
    
    //SgNodeHelper::replaceAstWithString((*i),"POINTER-INIT-ARRAY:"+(*i)->unparseToString()+"...;");
  }
  
  //list<pair<SgPntrArrRefExp*,ArrayElementAccessData> > 
  cout<<"STATUS: Replacing Expressions ... ";
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgAssignOp(*i)) {
      if(SgVarRefExp* lhsVar=isSgVarRefExp(SgNodeHelper::getLhs(*i))) {
        if(SgNodeHelper::isPointerVariable(lhsVar)) {
          //cout<<"DEBUG: pointer var on lhs :"<<(*i)->unparseToString()<<endl;

          SgExpression* rhsExp=isSgExpression(SgNodeHelper::getRhs(*i));
          ROSE_ASSERT(rhsExp);
          if(isSgAddressOfOp(rhsExp)) {
            rhsExp=isSgExpression(SgNodeHelper::getFirstChild(rhsExp));
            ROSE_ASSERT(rhsExp);
          }
          if(SgVarRefExp* rhsVar=isSgVarRefExp(rhsExp)) {
            VariableId lhsVarId=variableIdMapping->variableId(lhsVar);
            VariableId rhsVarId=variableIdMapping->variableId(rhsVar);
            SgExpressionPtrList& arrayInitializerList=variableIdMapping->getInitializerListOfArrayVariable(rhsVarId);
            //cout<<"DEBUG: rhs array:"<<arrayInitializerList.size()<<" elements"<<endl;
            int num=0;
            stringstream ss;
            for(SgExpressionPtrList::iterator j=arrayInitializerList.begin();j!=arrayInitializerList.end();++j) {
              ss<<variableIdMapping->variableName(lhsVarId)<<"_"<<num<<" = "
                <<(*j)->unparseToString();

              // workaround for the fact that the ROSE unparser generates a "\n;" for a replaced assignment
              {
                SgExpressionPtrList::iterator j2=j;
                j2++;
                if(j2!=arrayInitializerList.end())
                  ss<<";\n";
              }

              num++;
            }
            SgNodeHelper::replaceAstWithString(*i,ss.str());
            //SgNodeHelper::replaceAstWithString(*i,"COPY-ARRAY("+lhsVar->unparseToString()+"<="+rhsVar->unparseToString()+")");
          }
        } else {
          RoseAst subAst(*i);
          for(RoseAst::iterator j=subAst.begin();j!=subAst.end();++j) {
            if(SgNodeHelper::isArrayAccess(*j)) {
              //cout<<"DEBUG: arrays access on rhs of assignment :"<<(*i)->unparseToString()<<endl;
              transformArrayAccess(*j,analyzer->getVariableIdMapping());
            }
          }
        }
      }
    }
    if(SgNodeHelper::isCond(*i)) {
      RoseAst subAst(*i);
      for(RoseAst::iterator j=subAst.begin();j!=subAst.end();++j) {
        if(SgNodeHelper::isArrayAccess(*j)) {
          transformArrayAccess(*j,analyzer->getVariableIdMapping());
        }
      }
    }
  }
  cout<<" done."<<endl;
#if 0
  for(ArrayAccessInfoType::iterator i=arrayAccesses.begin();i!=arrayAccesses.end();++i) {
    //cout<<(*i).first->unparseToString()<<":"/*<<(*i).second.xxxx*/<<endl;
    ArrayElementAccessData arrayAccess=(*i).second;
    ROSE_ASSERT(arrayAccess.getDimensions()==1);
    VariableId accessVar=arrayAccess.getVariable();
    int accessSubscript=arrayAccess.getSubscript(0);
    // expression is now: VariableId[subscript]
    // information available is: arrayPointer map: VariableId:pointer -> VariableId:array
    // pointerArray

    // if the variable is not a pointer var it will now be added to the stl-map but with
    // a default VariableId. Default variableIds are not valid() IDs.
    // therefor this becomes a cheap check, whether we need to replace the expression or not.
    VariableId mappedVar=arrayPointer[accessVar];
    if(mappedVar.isValid()) {
      // need to replace
      stringstream newAccess;
#if 0
      newAccess<<variableIdMapping->variableName(mappedVar)<<"["<<accessSubscript<<"]";
#else
      newAccess<<variableIdMapping->variableName(accessVar)<<"_"<<accessSubscript<<" ";
#endif
      cout<<"to replace: @"<<(*i).first<<":"<<(*i).first->unparseToString()<<" ==> "<<newAccess.str()<<endl;
      SgNodeHelper::replaceAstWithString((*i).first,newAccess.str());
    }
  }
#endif
  Analyzer::VariableDeclarationList unusedGlobalVariableDeclarationList=analyzer->computeUnusedGlobalVariableDeclarationList(root);
  cout<<"STATUS: deleting unused global variables."<<endl;
  for(Analyzer::VariableDeclarationList::iterator i=unusedGlobalVariableDeclarationList.begin();
      i!=unusedGlobalVariableDeclarationList.end();
      ++i) {
    SgVariableDeclaration* decl=*i;
    SageInterface::removeStatement(decl);
  }
      
}
