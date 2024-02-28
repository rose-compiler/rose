#include "SageUtils.h"
#include <sageInterface.h>


using namespace Rose;
using namespace SageInterface;
using namespace Sawyer::Message::Common;


extern Sawyer::Message::Common::Facility mlog;

/**
 * walkupDots
 *
 * There's a bug in Qing's R/W sets in that it handles dot and arrow
 * expressions weirdly.  If you have a reference like a.b.c.d R/W sets
 * returns just a.b.  This makes it hard to get all the info we need.
 * So This recursive function walks up the dot expressions to get the
 * outtermost dot expression so we can interpret the whole reference.
 */
SgNode* SageUtils::walkupDots(SgNode* current) 
{
    SgNode* parent = current->get_parent();
    if(isSgDotExp(parent)) {
        return walkupDots(parent);
    }
    if(isSgArrowExp(parent)) {
        return walkupDots(parent);
    }
    return current;
}
//Add Cast and other types that need to be extracted
