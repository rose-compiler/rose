// Author: Markus Schordan
// $Id: AstClearVisitFlags.h,v 1.2 2005/09/10 22:48:07 dquinlan Exp $

#ifndef ASTCLEARVISITFLAGS_H
#define ASTCLEARVISITFLAGS_H

#if ASTTRAVERSAL_USE_VISIT_FLAG

class AstClearVisitFlags {
 public:
  void traverse(SgNode* node);
 protected:
  void visit(SgNode* node);
};

// endif for ASTTRAVERSAL_USE_VISIT_FLAG
#endif 

#endif
