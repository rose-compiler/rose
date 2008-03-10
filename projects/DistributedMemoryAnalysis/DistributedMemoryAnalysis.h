// This file is part of a new experimental framework for distributed memory program analysis using MPI.
// Author: Gergo Barany
// $Id: DistributedMemoryAnalysis.h,v 1.1 2008/01/08 02:55:52 dquinlan Exp $

#ifndef DISTRIBUTED_MEMORY_ANALYSIS_H
#define DISTRIBUTED_MEMORY_ANALYSIS_H

#include <mpi.h>
#include <rose.h>
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
  }
  virtual ~DistributedMemoryAnalysisBase() {}

protected:
  int myID() const { return my_rank; }
  int numberOfProcesses() const { return processes; }

    std::vector<SgFunctionDeclaration *> funcDecls;
    std::vector<InheritedAttributeType> initialInheritedValues;
    std::vector<int> functionsPerProcess;
    static const int root_process = 0;
    std::pair<int, int> computeFunctionIndices(SgNode *root,
                                               InheritedAttributeType rootInheritedValue,
                                               AstTopDownProcessing<InheritedAttributeType> *preTraversal);

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

#include "DistributedMemoryAnalysisImplementation.h"

#endif
