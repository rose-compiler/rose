/*
 * SageNodeAnnotation.hpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2011 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: SageNodeAnnotation.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef PLUTO_ROSE_SAGE_NODE_ANNOTATION_HPP
# define PLUTO_ROSE_SAGE_NODE_ANNOTATION_HPP

// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION


#include <rose.h>


#include <polyopt/PolyDDV.hpp>


// Annotation class for inner-most for loops that can be analyzed in
// the polyhedral model.
// isParallel = true if the loop is sync-free parallel
// isStrideOne = true if all memory references have stride 0 or 1.
// isMaybeAligned = false if we can prove statically the alignment
// expression is not a constant
class SageForInfo : public AstAttribute
{
public:
  int			isParallel;
  int			isMaybeAligned;
  int			isStrideOne;
  std::vector<PolyDDV>	ddvs;

  /// Default constructor
  SageForInfo() : isParallel(0), isMaybeAligned(0), isStrideOne(0) {}

  /// Copy constructor
  SageForInfo(SageForInfo const& Other) :
    isParallel(Other.isParallel),
    isMaybeAligned(Other.isMaybeAligned),
    isStrideOne(Other.isStrideOne),
    ddvs(Other.ddvs) {
    }

  virtual AstAttribute* copy() const {
    return new SageForInfo(*this);
  }

  static std::string name() { return std::string("SageForInfo"); }
};


// Annotation class for memory references.
// Each SgPntrArrRefExp in a vectorizable inner-most loop will contain
// this annotation. Other kinds of references are not annotated.
class SageMemoryReferenceInfo : public AstAttribute
{
public:
  int			isStrideOne;
  int			isStrideZero;
  int			isMaybeAligned;
  SgExpression*		alignmentExpression;

  /// Default constructor
  SageMemoryReferenceInfo() : isStrideOne(0),
			      isStrideZero(0),
			      isMaybeAligned(0),
			      alignmentExpression(NULL) {}

  /// Copy constructor
  SageMemoryReferenceInfo(SageMemoryReferenceInfo const& Other) :
    isStrideOne(Other.isStrideOne),
    isStrideZero(Other.isStrideZero),
    isMaybeAligned(Other.isMaybeAligned),
    alignmentExpression(NULL) {
      SgTreeCopy treeCopy;
      if(Other.alignmentExpression!=NULL)
        alignmentExpression =
          isSgExpression(Other.alignmentExpression->copy(treeCopy));
    }

  // Destructor
  ~SageMemoryReferenceInfo() {
    if (alignmentExpression)
      SageInterface::deepDelete(alignmentExpression);
  }

  virtual AstAttribute* copy() const {
    return new SageMemoryReferenceInfo(*this);
  }

  static std::string name() { return std::string("SageMemoryReferenceInfo"); }
};


///////////// Scop-specific annotations //////////////////
// This is used internally by polyopt.

class ScopForAnnotation : public AstAttribute
{
public:
  SgExpression*		lb;
  SgExpression*		ub;
  SgVariableSymbol*	iterator;
  int			astId;

  virtual AstAttribute* copy() const { return new ScopForAnnotation(); }
};


class ScopIfAnnotation : public AstAttribute
{
public:
  SgExpression* conditional;


  virtual AstAttribute* copy() const { return new ScopIfAnnotation(); }
};

class ScopStatementAnnotation : public AstAttribute
{
public:
  SgExpression* xxx;
  int astId;
  std::vector<SgNode*> readRefs;
  std::vector<SgNode*> writeRefs;
  std::vector<SgNode*> readAffineRefs;
  std::vector<SgNode*> writeAffineRefs;

  /// Default constructor
  ScopStatementAnnotation() {}


  /// Copy constructor
  ScopStatementAnnotation(ScopStatementAnnotation const& Other) :
    astId(Other.astId),
    readRefs(Other.readRefs),writeRefs(Other.writeRefs),
    readAffineRefs(Other.readAffineRefs),
    writeAffineRefs(Other.writeAffineRefs) {
    }


  virtual AstAttribute* copy() const { return new ScopStatementAnnotation(); }
};

class ScopRootAnnotation : public AstAttribute
{
public:
  std::set<std::pair<SgVariableSymbol*, std::vector<bool> > > arraysAsScalars;
  std::set<SgVariableSymbol*> linearizedArraysAsArrays;
  std::map<std::string, SgVariableSymbol*> fakeSymbolMap;
  int scopId;
  virtual AstAttribute* copy() const { return new ScopRootAnnotation(); }
};


class ScopParentAnnotation : public AstAttribute
{
public:
  std::vector<SgNode*> privateVars;
  virtual AstAttribute* copy() const { return new ScopParentAnnotation(); }
};



#endif
