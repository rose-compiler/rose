/* A test case for Qing's bool 
 * BuildCFGTraverse<Node>::ProcessGoto(AstInterface&, const AstNodePtr&, const AstNodePtr&)
 *
 * Originally from buildrose/include-staging/g++_HEADERS/hdrs2/bits/gthr-default.h
 *
 * It failed if the true body and false body of a if statement are returns without basic blocks
 *  ../../../../../rose/src/midend/programAnalysis/CFG/CFG.h:415: bool BuildCFGTraverse<Node>::ProcessGoto(AstInterface&, const AstNodePtr&, const AstNodePtr&) [with Node = ReachingDefNode]: Assertion `lastNode != 0' failed.
 * Liao, 11/18/2008
 * */
extern int __gthread_active_p();

int __gthread_once()
{
  if ((__gthread_active_p())) 
    return 0;
   else 
    return - 1;
// crashes here!!
}
