// Original Author (AstProcessing classes): Markus Schordan
// Rewritten by: Gergo Barany
// $Id: AstProcessing.h,v 1.6 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTPROCESSING_H
#define ASTPROCESSING_H

/*
   GB (06/01/2007):
   Changes to the original code were roughly:
   - removed unneeded includes and other code
   - removed everything related to visit flags
   - changed support for SynthesizedAttributesList; synthesized attributes are
     now handled in a smarter way, by keeping them on a stack and (in
     principle) only passing iterators instead of copying a container;
     traverse() is now only a thin wrapper around performTraversal() that
     takes care of visiting nodes and pushing/popping synthesized attributes
   - rewrote inFileToTraverse() to use new AST features instead of lots of
     string comparisons; it is now explicitly stated (at least in this
     comment) that we *do* visit subtrees from other files unless their root
     is in global or namespace scope, which is how we hope to avoid headers
     but still handle all included "code"
   - renamed class _DummyAttribute because that identifier is reserved for the
     implementation
   - everything that was not changed in some other way was totally
     reformatted, so diffing the old and new files won't do anything
     meaningful
   - appended "" suffix to pretty much any global identifier that would
     clash with the existing ones; if this code is ever moved into ROSE to
     replace the old version, it should not be exceedingly hard to remove the
     suffix everywhere
   - added some new virtual functions the user may choose to implement:
        atTraversalStart(), atTraversalEnd(), destroyInheritedValue()
   - other stuff I have probably forgotten
   GB (7/6/2007):
   - added new class AstPrePostProcessing and changed the traverseOrder
     flag so that it can now be pre and post order at the same time
 */

#include "AstSuccessorsSelectors.h"
#include "StackFrameVector.h"

// This type is used as a dummy template parameter for those traversals
// that do not use inherited or synthesized attributes.
typedef void *DummyAttribute;
// We initialize DummyAttributes to this value to avoid "may not be
// initialized" warnings. If you change the typedef, adjust the constant...
const static DummyAttribute defaultDummyAttribute = NULL;
// The attribute _DummyAttribute is reserved for the implementation, so we
// should deprecate it, but there is code using it explicitly. Which it
// shouldn't.
typedef DummyAttribute _DummyAttribute;

template <class InheritedAttributeType, class SynthesizedAttributeType>
class SgCombinedTreeTraversal;

// Base class for all traversals.
template <class InheritedAttributeType, class SynthesizedAttributeType>
class SgTreeTraversal
{
public:  
    typedef StackFrameVector<SynthesizedAttributeType> SynthesizedAttributesList;

    SynthesizedAttributeType traverse(SgNode* basenode,
            InheritedAttributeType inheritedValue,
            t_traverseOrder travOrder = preandpostorder);
    SynthesizedAttributeType traverseWithinFile(SgNode* basenode,
            InheritedAttributeType inheritedValue,
            t_traverseOrder travOrder = preandpostorder);
    void traverseInputFiles(SgProject* projectNode,
            InheritedAttributeType inheritedValue,
            t_traverseOrder travOrder = preandpostorder);

    // Default destructor/constructor
    virtual ~SgTreeTraversal();
    SgTreeTraversal();

    // Copy operations
    SgTreeTraversal(const SgTreeTraversal &);
    const SgTreeTraversal &operator=(const SgTreeTraversal &);

    friend class SgCombinedTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>;

#include "Cxx_GrammarTreeTraversalAccessEnums.h"

protected:
    virtual InheritedAttributeType evaluateInheritedAttribute(SgNode* astNode,
            InheritedAttributeType inheritedValue) = 0;
    virtual SynthesizedAttributeType evaluateSynthesizedAttribute(SgNode* n,
            InheritedAttributeType in,
            SynthesizedAttributesList l) = 0;
    typedef typename AstSuccessorsSelectors::SuccessorsContainer SuccessorsContainer;
    typedef SuccessorsContainer& SuccessorsContainerRef;

    // GB (09/25/2007): Feel free to override this to implement custom traversals, but see the
    // comment for set_useDefaultIndexBasedTraversal() below!
    virtual void setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer);

    virtual SynthesizedAttributeType defaultSynthesizedAttribute(InheritedAttributeType inh);

    // GB (06/04/2007): A new virtual function called at the start of the
    // traversal, before any node is actually visited; can be used to
    // compute attributes that may have changed since the constructor was
    // executed, but are constant during the traversal itself. A no-op by
    // default. If you don't know what you would use this for, ignore it.
    virtual void atTraversalStart();
    // GB (06/13/2007): Added this just for symmetry, not sure if it is
    // useful, but it won't hurt to have it.
    virtual void atTraversalEnd();

    // GB (09/25/2007): This flag determines whether the new index-based traversal mechanism or the more general
    // mechanism based on successor containers is to be used. Indexing should be faster, but it would be quite hard to
    // adapt it to the reverse traversal and other specialized traversals. Thus: This is true by default, and anybody
    // who overrides setNodeSuccessors() *must* change this to false to force the traversal to use their custom
    // successor container.
    void set_useDefaultIndexBasedTraversal(bool);

private:
    bool inFileToTraverse(SgNode* node);
    void performTraversal(SgNode *basenode,
            InheritedAttributeType inheritedValue,
            t_traverseOrder travOrder);
    SynthesizedAttributeType traversalResult();

    bool useDefaultIndexBasedTraversal;
    bool traversalConstraint;
    SgFile *fileToVisit;

    // stack of synthesized attributes; evaluateSynthesizedAttribute() is
    // automagically called with the appropriate stack frame, which
    // behaves like a non-resizable std::vector
    SynthesizedAttributesList *synthesizedAttributes;
};

template <class InheritedAttributeType, class SynthesizedAttributeType>
class AstCombinedTopDownBottomUpProcessing;

template <class InheritedAttributeType, class SynthesizedAttributeType>
class AstTopDownBottomUpProcessing
    : public SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>
{
public:
    typedef typename SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>
        ::SynthesizedAttributesList SynthesizedAttributesList;

    // deprecated
    typedef SynthesizedAttributesList SubTreeSynthesizedAttributes;

    //! evaluates attributes on the entire AST
    SynthesizedAttributeType traverse(SgNode* node, InheritedAttributeType inheritedValue);

    //! evaluates attributes only at nodes which represent the same file as where the evaluation was started
    SynthesizedAttributeType traverseWithinFile(SgNode* node, InheritedAttributeType inheritedValue);

    friend class AstCombinedTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>;

protected:
    //! pure virtual function which must be implemented to compute the inherited attribute at a node
    virtual InheritedAttributeType evaluateInheritedAttribute(SgNode* astNode,
            InheritedAttributeType inheritedValue) = 0;
    //! pure virtual function which must be implemented to compute the synthesized attribute at a node. The list of
    //! synthesized attributes consists of the synthesized attributes computed at the children node of the current node.
    //! The inherited attribute value is computed by evaluateInheritedAttribute at the same node and simply passed to this function.
    //! Use the typedef SynthesizedAttributeList as type for the synthesized attributes list.
    virtual SynthesizedAttributeType evaluateSynthesizedAttribute(SgNode*,
            InheritedAttributeType,
            SynthesizedAttributesList) = 0;

    //! Function called at the start of the traversal, before any node is
    //! visited; override if necessary, the default implementation is a
    //! no-op.
    virtual void atTraversalStart();
    virtual void atTraversalEnd();
};

template <class InheritedAttributeType>
class AstCombinedTopDownProcessing;

template <class InheritedAttributeType>
class DistributedMemoryAnalysisPreTraversal;

template <class InheritedAttributeType>
class AstTopDownProcessing
    : public SgTreeTraversal<InheritedAttributeType, DummyAttribute>
{
public:
    typedef typename SgTreeTraversal<InheritedAttributeType, DummyAttribute>
        ::SynthesizedAttributesList SynthesizedAttributesList;

    //! evaluates attributes on the entire AST
    void traverse(SgNode* node, InheritedAttributeType inheritedValue);

    //! evaluates attributes only at nodes which represent the same file as where the evaluation was started
    void traverseWithinFile(SgNode* node, InheritedAttributeType inheritedValue);

    friend class AstCombinedTopDownProcessing<InheritedAttributeType>;
    friend class DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>;

protected:
    //! pure virtual function which must be implemented to compute the inherited attribute at a node
    virtual InheritedAttributeType evaluateInheritedAttribute(SgNode* astNode,
            InheritedAttributeType inheritedValue) = 0;

    //! Function called at the start of the traversal, before any node is
    //! visited; override if necessary, the default implementation is a
    //! no-op.
    virtual void atTraversalStart();
    virtual void atTraversalEnd();
    // GB (06/04/2007): This is a new virtual function, a no-op by
    // default. It is called for every node, after its successors have
    // been visited, with the inherited attribute computed at this node.
    // The intention is to be able to free any memory (or other resources)
    // allocated by evaluateInheritedAttribute().
    virtual void destroyInheritedValue(SgNode*, InheritedAttributeType);

private:
    DummyAttribute evaluateSynthesizedAttribute(SgNode* astNode,
            InheritedAttributeType inheritedValue,
            SynthesizedAttributesList l);
    DummyAttribute defaultSynthesizedAttribute(InheritedAttributeType inh);
};

template <class SynthesizedAttributeType>
class AstCombinedBottomUpProcessing;

template <class InheritedAttributeType>
class DistributedMemoryAnalysisPostTraversal;

template <class SynthesizedAttributeType>
class AstBottomUpProcessing
    : public SgTreeTraversal<DummyAttribute,SynthesizedAttributeType>
{
public:
    typedef typename SgTreeTraversal<DummyAttribute, SynthesizedAttributeType>
        ::SynthesizedAttributesList SynthesizedAttributesList;
 
    // deprecated
    typedef SynthesizedAttributesList SubTreeSynthesizedAttributes; 

    //! evaluates attributes on the entire AST
    SynthesizedAttributeType traverse(SgNode* node);

    //! evaluates attributes only at nodes which represent the same file as where the evaluation was started
    SynthesizedAttributeType traverseWithinFile(SgNode* node);

    //! evaluates attributes only at nodes which represent files which were specified on the command line (=input files).
    void traverseInputFiles(SgProject* projectNode);

    friend class AstCombinedBottomUpProcessing<SynthesizedAttributeType>;
    friend class DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>;

protected:
    //! pure virtual function which must be implemented to compute the synthesized attribute at a node. The list of
    //! synthesized attributes consists of the synthesized attributes computed at the children node of the current node.
    //! The inherited attribute value is computed by the function evaluateInheritedAttribute at the same node and simply passed to this function.
    //! Use the typedef SynthesizedAttributeList as type for the synthesized attributes list. 
    virtual SynthesizedAttributeType evaluateSynthesizedAttribute(SgNode*, SynthesizedAttributesList) = 0;

    //! Allows to provide a default value for a synthesized attribute of primitive type (e.g. int, bool, etc.).
    //! If a class is used as type for a synthesized attribute the default constructor of this class is sufficient and this function does not have be
    //! implemented.
    virtual SynthesizedAttributeType defaultSynthesizedAttribute();

    //! Function called at the start of the traversal, before any node is
    //! visited; override if necessary, the default implementation is a
    //! no-op.
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

private:
    virtual DummyAttribute evaluateInheritedAttribute(SgNode* astNode, DummyAttribute inheritedValue);
    virtual SynthesizedAttributeType evaluateSynthesizedAttribute(SgNode* astNode, DummyAttribute inheritedValue, SynthesizedAttributesList l);
    virtual SynthesizedAttributeType defaultSynthesizedAttribute(DummyAttribute inheritedValue);
};

// deprecated classes (provided for compatibility with existing user code - will be removed at some point in future)
class AstSynthesizedAttribute {};
class AstInheritedAttribute {};
class SgSynthesizedAttribute : public AstSynthesizedAttribute {};
class SgInheritedAttribute : public AstInheritedAttribute {};
template <class InheritedAttributeType, class SynthesizedAttributeType>
class SgTopDownBottomUpProcessing : public AstTopDownBottomUpProcessing <InheritedAttributeType, SynthesizedAttributeType> {};
template <class InheritedAttributeType>
class SgTopDownProcessing : public AstTopDownProcessing <InheritedAttributeType> {};
template <class SynthesizedAttributeType>
class SgBottomUpProcessing : public AstBottomUpProcessing <SynthesizedAttributeType> {};

// Add preprocessor code to include the source file where explicit template instantiation is required 
// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
   #include "AstProcessing.C" 
// #endif 

#include "AstSimpleProcessing.h" // that's a non-templated class which is put in a different file (for gcc to compile&link properly)

#include "AstCombinedProcessing.h"

#include "AstSharedMemoryParallelProcessing.h"

#endif
