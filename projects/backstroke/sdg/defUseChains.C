#include "defUseChains.h"
#include <VariableRenaming.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH


namespace SDG
{



void generateDefUseChainsFromVariableRenaming(SgProject* project, DefUseChains& defUseChains)
{
    VariableRenaming varRenaming(project);
    varRenaming.run();

    const VariableRenaming::DefUseTable& useTable = varRenaming.getUseTable();
    foreach (const VariableRenaming::DefUseTable::value_type& usesOnNode, useTable)
    {
        foreach (const VariableRenaming::TableEntry::value_type& entry, usesOnNode.second)
        {
            foreach (SgNode* node, entry.second)
            {
                defUseChains[node].insert(usesOnNode.first);
            }
        }
    }
}



} // end of namespace
