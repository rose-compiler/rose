// This file is part of a new experimental framework for distributed memory program analysis using MPI.
// Author: Gergo Barany
// $Id: DistributedMemoryAnalysis.h,v 1.1 2008/01/08 02:55:52 dquinlan Exp $

#ifndef DISTRIBUTED_MEMORY_ANALYSIS_H
#define DISTRIBUTED_MEMORY_ANALYSIS_H

#include <rose.h>
#include <utility>

void initializeDistributedMemoryProcessing(int *argc, char ***argv);
void finalizeDistributedMemoryProcessing();

template <class InheritedAttributeType>
class DistributedMemoryAnalysisBase
{
public:
    bool isRootProcess() const;
    DistributedMemoryAnalysisBase();
    ~DistributedMemoryAnalysisBase();

protected:
    int myID() const;
    int numberOfProcesses() const;

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
    SynthesizedAttributeType getFinalResults();
    DistributedMemoryTraversal();
    virtual ~DistributedMemoryTraversal();

protected:
    virtual SynthesizedAttributeType analyzeSubtree(SgFunctionDeclaration *funcDecl,
                                                    InheritedAttributeType initialInheritedValue) = 0;
    virtual std::pair<int, void *> serializeAttribute(SynthesizedAttributeType attribute) const = 0;
    virtual SynthesizedAttributeType deserializeAttribute(std::pair<int, void *> serializedAttribute) const = 0;
    virtual void deleteSerializedAttribute(std::pair<int, void *> serializedAttribute) const;

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
    DistributedMemoryAnalysisPreTraversal(AstTopDownProcessing<InheritedAttributeType> *);

    std::vector<SgFunctionDeclaration *> &get_funcDecls();
    std::vector<InheritedAttributeType> &get_initialInheritedValues();
    std::vector<size_t> &get_nodeCounts();

protected:
    InheritedAttributeType evaluateInheritedAttribute(SgNode *, InheritedAttributeType);
    void destroyInheritedValue(SgNode *, InheritedAttributeType);

private:
    AstTopDownProcessing<InheritedAttributeType> *preTraversal;
    bool inFunc;
    size_t nodeCount;
    std::vector<SgFunctionDeclaration *> funcDecls;
    std::vector<InheritedAttributeType> initialInheritedValues;
    std::vector<size_t> nodeCounts;
};

template <class SynthesizedAttributeType>
class DistributedMemoryAnalysisPostTraversal
  : public AstTopDownBottomUpProcessing<bool, SynthesizedAttributeType>
{
public:
    DistributedMemoryAnalysisPostTraversal(AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal,
                                           const std::vector<SynthesizedAttributeType> &functionResults);
    typedef typename AstTopDownBottomUpProcessing<bool, SynthesizedAttributeType>::SynthesizedAttributesList SynthesizedAttributesList;

protected:
    bool evaluateInheritedAttribute(SgNode *, bool);
    SynthesizedAttributeType evaluateSynthesizedAttribute(SgNode *, bool, SynthesizedAttributesList);
    SynthesizedAttributeType defaultSynthesizedAttribute(bool);

private:
    AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal;
    const std::vector<SynthesizedAttributeType> &functionResults;
    int functionCounter;
};


#if 0
// DQ (9/28/2007): Commented out to avoid use, this is the old interface.

// treat this class as deprecated; it is mainly here for historical reasons

template <class InheritedAttributeType>
class DistributedMemoryAnalysis: public DistributedMemoryAnalysisBase<InheritedAttributeType>
{
public:
    void performAnalysis(SgNode *root,
                         InheritedAttributeType rootInheritedValue,
                         AstTopDownProcessing<InheritedAttributeType> *preTraversal);
    DistributedMemoryAnalysis();
    virtual ~DistributedMemoryAnalysis();

protected:
    virtual void analyzeSubtree(SgFunctionDeclaration *funcDecl,
                                InheritedAttributeType initialInheritedValue) = 0;
    virtual std::pair<int, void *> serialize() = 0;
    virtual void addSerializedState(std::pair<int, void *>) = 0;
    virtual void cleanupSerializedState(std::pair<int, void *>);

private:
    DistributedMemoryAnalysis(const DistributedMemoryAnalysis &);
    const DistributedMemoryAnalysis &operator=(const DistributedMemoryAnalysis &);
};
#endif

#include "DistributedMemoryAnalysisTemplateDefs.C"

#endif
