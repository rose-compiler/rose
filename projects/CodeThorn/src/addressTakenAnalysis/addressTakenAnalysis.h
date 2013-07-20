#ifndef _ADDRESSTAKENANALYSIS_H
#define _ADDRESSTAKENANALYSIS_H

/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/

#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"
#include <set>

using namespace CodeThorn;

typedef std::set<VariableId> VariableIdSet;

// temporary utility function
// will be replaced by set_union from algorithm
void set_union(const VariableIdSet& set1, const VariableIdSet& set2, VariableIdSet& rset);

// AST Query Processor
// common functor to process any query and build match result
// NOTE: extend it to accept a functor to apply on each element of match result

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/
class ProcessQuery
{
  // the root node on which the AST matching needs to be performed
  MatchResult match_result;

public:
  ProcessQuery() { }
  // functor to operate on the given query
  MatchResult operator()(std::string query, SgNode* root);
  MatchResult getMatchResult();
  void printMatchResult();
  void clearMatchResult();
};

// wrapper for set<VariableId> with pretty printing
// 
class VariableIdSetPrettyPrint
{
public:
  std::string static str(VariableIdSet& vset, VariableIdMapping& vidm)
  {
    std::ostringstream ostr;
    ostr << "[";
    VariableIdSet::iterator it = vset.begin();
    for( ; it != vset.end(); )
    {
      ostr << "<" << (*it).toString() << ", " << vidm.variableName(*it)  << ">";
      it++;
      if(it != vset.end())
        ostr << ", ";
    }
    ostr << "]";
    return ostr.str();
  }
  std::string static str(VariableIdSet& vset)
  {
    std::ostringstream ostr;
    ostr << "[";
    VariableIdSet::iterator it = vset.begin();
    for( ; it != vset.end(); )
    {
      ostr << (*it).toString();
      it++;
      if(it != vset.end())
        ostr << ", ";
    }
    ostr << "]";
    return ostr.str();
  }
};

/*************************************************
 ************** AddressTakenAnalysis  ************
 *************************************************/
class AddressTakenAnalysis
{
  VariableIdMapping& vidm;
  // result to be computed by this analysis
  VariableIdSet addressTakenSet;
  bool initialized;

  // address can be taken for any expression that is lvalue
  // The purpose of this class is to traverse arbitrary
  // expressions that are operands of SgAddressOfOp and find the
  // variable whose address is actually taken.
  // For example in expression &(a.b->c),  'c' address is
  // actually taken. This class simply traverses the rhs_operand
  // of SgDotExp/SgArrowExp or other expressions to identify 
  // the variable whose address is taken
  // 
  class OperandToVariableId : public ROSE_VisitorPatternDefaultBase
  {
    AddressTakenAnalysis& ata;
  public:
    OperandToVariableId(AddressTakenAnalysis& _ata) : ata(_ata) { }
    void visit(SgVarRefExp*);
    void visit(SgDotExp*);
    void visit(SgArrowExp*);
    void visit(SgPointerDerefExp*);
    void visit(SgPntrArrRefExp*);
    void visit(SgNode* sgn);
  };
public:
  AddressTakenAnalysis(VariableIdMapping& _vidm) : vidm(_vidm), initialized(true) {}
  void throwIfUnInitException();
  void computeAddressTakenSet(SgNode* root);
  void printAddressTakenSet();
  VariableIdSet getAddressTakenSet();
};

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/
class TypeAnalysis
{
  VariableIdMapping& vidm;
  VariableIdSet pointerTypeSet;
  VariableIdSet arrayTypeSet;
  bool initialized;

public:
  TypeAnalysis(VariableIdMapping& _vidm) : vidm(_vidm) { }
  void initialize(VariableIdMapping& vidm);
  void collectTypes();
  void printPointerTypeSet();
  void printArrayTypeSet();
  void throwIfUnInitException();
  VariableIdSet getPointerTypeSet();
  VariableIdSet getArrayTypeSet();
};

/*************************************************
 ******** FlowInsensitivePointerAnalysis  ********
 *************************************************/
class FlowInsensitivePointerAnalysis
{
  SgNode* root;
  VariableIdMapping vidm;
  AddressTakenAnalysis addrTakenAnalysis;
  TypeAnalysis typeAnalysis;

public:
  FlowInsensitivePointerAnalysis(SgProject* project, VariableIdMapping& _vidm) : root(project), 
                                                                                 vidm(_vidm),
                                                                                 addrTakenAnalysis(vidm),
                                                                                 typeAnalysis(vidm)
  { 
  }
  void runAnalysis();
  void printAnalysisSets();
  VariableIdSet getMemModByPointer();
  VariableIdMapping& getVariableIdMapping();
};

#endif
