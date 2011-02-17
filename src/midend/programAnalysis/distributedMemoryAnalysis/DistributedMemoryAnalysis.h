// This file is part of a new experimental framework for distributed memory program analysis using MPI.
// Author: Gergo Barany
// $Id: DistributedMemoryAnalysis.h,v 1.1 2008/01/08 02:55:52 dquinlan Exp $

#ifndef DISTRIBUTED_MEMORY_ANALYSIS_H
#define DISTRIBUTED_MEMORY_ANALYSIS_H

#if ROSE_MPI

//#include <mpi.h>

#include <utility>

void initializeDistributedMemoryProcessing(int *argc, char ***argv);
void finalizeDistributedMemoryProcessing();

template <class InheritedAttributeType>
class DistributedMemoryAnalysisBase
{
public:
  bool isRootProcess() const {return (my_rank == root_process);}
  DistributedMemoryAnalysisBase() {
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    nrOfNodes=0;
  }
  virtual ~DistributedMemoryAnalysisBase() {}

public:
  int myID() const { return my_rank; }
  int numberOfProcesses() const { return processes; }

  int nrOfNodes;
  std::vector<size_t> myNodeCounts;
  std::vector<size_t> myFuncWeights;

  std::vector<SgFunctionDeclaration *> funcDecls;
  std::vector<InheritedAttributeType> initialInheritedValues;
  std::vector<int> functionsPerProcess;
  static const int root_process = 0;
  std::pair<int, int> computeFunctionIndices(SgNode *root,
                                             InheritedAttributeType rootInheritedValue,
                                             AstTopDownProcessing<InheritedAttributeType> *preTraversal);
  void computeFunctionIndicesPerNode(SgNode *root, std::vector<int>& functionToProcessor,  
                                     InheritedAttributeType rootInheritedValue,
                                     AstTopDownProcessing<InheritedAttributeType> *preTraversal);
  void sortFunctions(std::vector<SgFunctionDeclaration*>& funcDecls, std::vector<InheritedAttributeType>& inhertiedValues,
                     std::vector<size_t>& nodeCounts, std::vector<size_t>& funcWeights);
  
 private:
  int my_rank;
  int processes;
};




template <class InheritedAttributeType, class SynthesizedAttributeType>
class DistributedMemoryTraversal: public DistributedMemoryAnalysisBase<InheritedAttributeType>
{
public:
    void performAnalysis(SgNode *root, InheritedAttributeType rootInheritedValue,
                         AstTopDownProcessing<InheritedAttributeType> *preTraversal,
                         AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal);
    SynthesizedAttributeType getFinalResults() {return finalResults;}
    DistributedMemoryTraversal() {}
    virtual ~DistributedMemoryTraversal() {}

protected:
    virtual SynthesizedAttributeType analyzeSubtree(SgFunctionDeclaration *funcDecl,
                                                    InheritedAttributeType initialInheritedValue) = 0;
    virtual std::pair<int, void *> serializeAttribute(SynthesizedAttributeType attribute) const = 0;
    virtual SynthesizedAttributeType deserializeAttribute(std::pair<int, void *> serializedAttribute) const = 0;
    virtual void deleteSerializedAttribute(std::pair<int, void *> serializedAttribute) const {}

private:
    SynthesizedAttributeType finalResults;
    std::vector<SynthesizedAttributeType> functionResults;

    DistributedMemoryTraversal(const DistributedMemoryTraversal &);
    const DistributedMemoryTraversal &operator=(const DistributedMemoryTraversal &);
};












// --------- Implementor Line - Do Not Cross ---------
// There is nothing for users to see here, move along.

template <class InheritedAttributeType>
class DistributedMemoryAnalysisPreTraversal
  : public AstTopDownProcessing<InheritedAttributeType>
{
public:
 DistributedMemoryAnalysisPreTraversal(AstTopDownProcessing<InheritedAttributeType> *preTraversal)
   : preTraversal(preTraversal), inFunc(false), nodeCount(0), stdFunc(false), weightNullDeref(1),
    weightAssignOp(1) {}

  std::vector<SgFunctionDeclaration *> &get_funcDecls() {return funcDecls;}
  std::vector<InheritedAttributeType> &get_initialInheritedValues() {return initialInheritedValues;}
  std::vector<size_t> &get_nodeCounts() {return nodeCounts;}
  std::vector<size_t> &get_funcWeights() {return funcWeights;}

protected:
    InheritedAttributeType evaluateInheritedAttribute(SgNode *, InheritedAttributeType);
    void destroyInheritedValue(SgNode *, InheritedAttributeType);

private:
    AstTopDownProcessing<InheritedAttributeType> *preTraversal;
    bool inFunc;
    size_t nodeCount;
    bool stdFunc;
    size_t weightNullDeref;
    size_t weightAssignOp;

    std::vector<SgFunctionDeclaration *> funcDecls;
    std::vector<InheritedAttributeType> initialInheritedValues;
    std::vector<size_t> nodeCounts;
    std::vector<size_t> funcWeights;
};




//namespace DistributedMemoryAnalysisNamespace {
//  bool postTraversalEvaluateInheritedAttribute(SgNode* node, bool inFunction);
//}

template <class SynthesizedAttributeType>
class DistributedMemoryAnalysisPostTraversal
  : public AstTopDownBottomUpProcessing<bool, SynthesizedAttributeType>
{
public:
    DistributedMemoryAnalysisPostTraversal(AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal,
                                           const std::vector<SynthesizedAttributeType> &functionResults)
      : postTraversal(postTraversal), functionResults(functionResults), functionCounter(0), stdFunc(false) {}

    typedef typename AstTopDownBottomUpProcessing<bool, SynthesizedAttributeType>::SynthesizedAttributesList SynthesizedAttributesList;

protected:
    bool evaluateInheritedAttribute(SgNode *node, bool inFunction);
    // {
    //  return DistributedMemoryAnalysisNamespace::postTraversalEvaluateInheritedAttribute(node, inFunction);
    //}
    SynthesizedAttributeType evaluateSynthesizedAttribute(SgNode *, bool, SynthesizedAttributesList);
    SynthesizedAttributeType defaultSynthesizedAttribute(bool) {return postTraversal->defaultSynthesizedAttribute();}

private:
    AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal;
    const std::vector<SynthesizedAttributeType> &functionResults;
    int functionCounter;
    bool stdFunc;
};












#include "DistributedMemoryAnalysisImplementation.h"

#endif

#endif
