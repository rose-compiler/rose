// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprLabeler.h,v 1.4 2008-06-26 08:08:09 gergo Exp $

#ifndef H_EXPRLABELER
#define H_EXPRLABELER

#include <satire_rose.h>

#include "cfg_support.h"
#include "IrCreation.h"

// preorder -- but it doesn't really matter
class ExprLabeler : public AstSimpleProcessing
{
public:
    ExprLabeler(int, CFG *, Procedure *);
    int get_expnum() const;

protected:
    void visit(SgNode *);

private:
    ExprLabeler();
    int expnum;
    CFG *cfg;
    Procedure *proc;
};

#endif
