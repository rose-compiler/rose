/* Liao, 5/14/2009
 * A test case from gcc
 * elements of an enum type are coming from a include file
 * They should not be unparsed to the file including the header
 * The todo item is depending on 
 *  bug 347 
 *  https://outreach.scidac.gov/tracker/index.php?func=detail&aid=347&group_id=24&atid=185
 * */

#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) LENGTH,

static const int c_tree_code_length[] = {
    0,
#include "c-common.def"
};
#undef DEFTREECODE
