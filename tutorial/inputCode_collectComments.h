/* comment from inputCode_collectComments.h */

// common MAX macro definition
#define max(a,b) ((a) > (b) ? (a) : (b))

// More efficent version using g++ statement expression extension
#define maxint(a,b) ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

int variable_with_comments_and_directives_attached;
