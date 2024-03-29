#ifndef SAGE_UTILS_H
#define SAGE_UTILS_H

#include <limits>
#include <vector>

namespace SageUtils 
{
    /**
     * walkupDots
     *
     * There's a bug in Qing's R/W sets in that it handles dot and arrow
     * expressions weirdly.  If you have a reference like a.b.c.d R/W sets
     * returns just a.b.  This makes it hard to get all the info we need.
     * So This recursive function walks up the dot expressions to get the
     * outtermost dot expression so we can interpret the whole reference.
     */
    SgNode* walkupDots(SgNode* current);
    

    
}


#endif
