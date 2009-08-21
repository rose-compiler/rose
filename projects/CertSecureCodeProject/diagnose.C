/**
 * \file diagnose.C
 *
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is
 * hereby granted, provided that the above copyright notice appear and that
 * both that copyright notice and this permission notice appear in supporting
 * documentation, and that the name of CMU not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WSTRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include <iostream>
#include "rose.h"
#include "utilities.h"

extern bool PREPRO(const SgNode *node);
extern bool DCL(const SgNode *node);
extern bool EXP(const SgNode *node);
extern bool ARR(const SgNode *node);
extern bool FLP(const SgNode *node);
extern bool INT(const SgNode *node);
extern bool STR(const SgNode *node);
extern bool MEM(const SgNode *node);
extern bool FIO(const SgNode *node);
extern bool ENV(const SgNode *node);
extern bool SIG(const SgNode *node);
extern bool ERR(const SgNode *node);
extern bool MSC(const SgNode *node);
extern bool POS(const SgNode *node);

class visitorTraversal : public AstSimpleProcessing {
public :
  visitorTraversal () {}
  virtual void visit(SgNode* node) {
    PREPRO(node);
    DCL(node);
    EXP(node);
    ARR(node);
    FLP(node);
    INT(node);
    STR(node);
    MEM(node);
    FIO(node);
    ENV(node);
    SIG(node);
    ERR(node);
    MSC(node);
    POS(node);
  }
};


int main( int argc, char* argv[]) {
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT( project );
  visitorTraversal exampleTraversal;
  exampleTraversal.traverseInputFiles( project, preorder);
  return 0;
}
