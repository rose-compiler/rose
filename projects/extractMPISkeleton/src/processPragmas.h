#ifndef __processPragmas_H_LOADED__
#define __processPragmas_H_LOADED__

#include "rose.h"

typedef std::set<SgSymbol*> SgSymbols;

void processPragmas ( SgProject *project
                    , bool outline
                    , SgSymbols *pivots
                    );

#endif

