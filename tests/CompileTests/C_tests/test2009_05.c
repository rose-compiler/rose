/* Liao, 5/14/2009
 * A test case from gcc
 * elements of an enum type are coming from a include file
 * They should not be unparsed to the file including the header
 * The todo item is depending on 
 *  bug 347 
 *  https://outreach.scidac.gov/tracker/index.php?func=detail&aid=347&group_id=24&atid=185
 * */

enum builtin_type {
  firstone,
#define DEF_PRIMITIVE_TYPE(NAME, VALUE) NAME,
#include "builtin-types.def"
  lastone
};

/* Tree code classes.  */
#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) TYPE,
static const char c_tree_code_type[] = {
  'z',
// test this  
  'x',
#include "c-common.def"
};
#undef DEFTREECODE

static const char c_tree_code_type2[] = {  'z',  'x'};
