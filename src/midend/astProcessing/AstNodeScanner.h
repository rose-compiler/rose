#ifndef ASTNODESCANNER_H
#define ASTNODESCANNER_H

#include "sgnodes.hpp"
#include "sgnodec.hpp"
#include "cr_scan.hpp"
#include "sage3.h"
#include "roseInternal.h"

#define SCANNER  CRScanner


int cocoAstNodeCode(SgNode* node);

extern SgNode* currentAstNode;

/* note: cr_abs.hpp:    AbsToken CurrSym;      // current (most recently parsed) token */


class AstToken {
 public:
  AstToken(int sym=No_Sym, SgNode* node=0) {
    scannerSymbol=sym;
    this->node=node;
  }
  int scannerSymbol;
  SgNode* node;
};


class __Dummy { public: __Dummy(){} };

class ATContainer : public std::vector<AstToken>, public AstTopDownBottomUpProcessing<__Dummy*,__Dummy*> {
 public:
  // no default constructor
  // ATContainer(SgProject* node); // create container
  //ATContainer(SgFile* node); // create container
  ATContainer(SgNode* node); // create container
 protected:
  __Dummy* evaluateInheritedAttribute(SgNode* astNode, __Dummy* d);
  __Dummy* evaluateSynthesizedAttribute(SgNode* astNode, __Dummy* d, SubTreeSynthesizedAttributes l);
};


class slangScanner : public SCANNER
{
public:
  slangScanner(int* SrcFile, int ignoreCase) { };
  slangScanner(SgNode* node);
  ~slangScanner();
  virtual void Reset();

  int Get();
  AstToken getCurrentAstToken();
  AstToken getLookAheadAstToken();

protected:
  static int STATE0[];
  int CheckLiteral(int id);
  int LeftContext(char *s);
  int Comment();

private:
  ATContainer* tokenstream;
  ATContainer::iterator inputIteratorStart;
  ATContainer::iterator inputIteratorEnd;
  //AstToken xcurrentAstToken;
  AstToken currentAstToken;
  AstToken lookAheadAstToken;
};

#endif /* ASTNODESCANNER_H */


