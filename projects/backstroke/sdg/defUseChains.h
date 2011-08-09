#ifndef SDG_DEFUSECHAINS_H
#define	SDG_DEFUSECHAINS_H

#include <rose.h>


namespace SDG
{

typedef std::map<SgNode*, std::set<SgNode*> > DefUseChains;

void generateDefUseChainsFromVariableRenaming(SgProject*, DefUseChains&);

void generateDefUseChainsFromSSA(SgProject*, DefUseChains&);


} // end of namespace

#endif	/* SDG_DEFUSECHAINS_H */

