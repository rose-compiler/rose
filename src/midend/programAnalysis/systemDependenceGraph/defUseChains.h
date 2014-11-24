/* 
 * File:   defUseChains.h
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#ifndef SDG_DEFUSECHAINS_H
#define SDG_DEFUSECHAINS_H

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "sage3basic.h"

#include "SDG.h"


namespace SDG
{

void generateDefUseChainsFromVariableRenaming(SgProject*, DefUseChains&);

void generateDefUseChainsFromSSA(SgProject*, DefUseChains&);


} // end of namespace

#endif /* SDG_DEFUSECHAINS_H */

