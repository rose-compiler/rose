#ifndef SDG_DEFUSECHAINS_H
#define	SDG_DEFUSECHAINS_H

#include <rose.h>
#include "SDG.h"


namespace SDG
{

void generateDefUseChainsFromVariableRenaming(SgProject*, DefUseChains&);

void generateDefUseChainsFromSSA(SgProject*, DefUseChains&);


} // end of namespace

#endif	/* SDG_DEFUSECHAINS_H */

