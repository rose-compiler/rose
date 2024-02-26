#include <iostream>
#include "ProcessAstTree.h"
#include "AstInterface_ROSE.h"
#include <ROSE_ASSERT.h>

#ifdef TEMPLATE_ONLY
template <class AstNodePtr>
void ProcessAstTreeBase<AstNodePtr>:: SetLocation(TraverseLocation state)
   {
     scopeStack.First()->GetEntry().state = state;
   }
template <class AstNodePtr>
typename ProcessAstTreeBase<AstNodePtr>::TraverseState 
ProcessAstTreeBase<AstNodePtr>:: GetScope() const
   {
     return (scopeStack.size() > 0)? scopeStack.First()->GetEntry()
                     : TraverseState(AstNodePtr(), 0, BEFORE);
   }
template <class AstNodePtr>
void ProcessAstTreeBase<AstNodePtr>:: PopScope()
   {
     scopeStack.PopFirst();
   }
template <class AstNodePtr>
void ProcessAstTreeBase<AstNodePtr>::
PushScope( const AstNodePtr& scope, int skip, TraverseLocation state)
   {
     TraverseState tmp(scope, skip, state);
     scopeStack.PushFirst(tmp);
   }
template <class AstNodePtr>
bool ProcessAstTreeBase<AstNodePtr>::
Traverse( AstInterface &fa, const AstNodePtr& s,
                      AstInterface::TraversalVisitType t)
   {
     cur = s;
     bool pass = true, result = true;
     TraverseState state = GetScope();
     if (state.scope == s) {
        state.state = state.state << 1;
        if (t == AstInterface::PreVisit)
           SetLocation( (TraverseLocation)state.state );
        else
           PopScope();
        if (state.skip & AT)
            pass = false;
        if (state.state == AT && t == AstInterface::PreVisit) {
           state.state = state.state << 1;
           SetLocation( (TraverseLocation)state.state );
        }
     }
     else if (state.scope != AstNodePtr() && (state.skip & state.state) )
         pass = false;
     if (pass) {
        result = ProcessTree(fa, s, t);
     }
     return result;
   }
template <class AstNodePtr>
void ProcessAstTreeBase<AstNodePtr>:: Skip( const AstNodePtr& s)
     {
        if (cur == s)  {
            PushScope(s, INSIDE, INSIDE);
        }
        else {
            PushScope(s, INSIDE_AT, BEFORE);
        }
     }
template <class AstNodePtr>
void ProcessAstTreeBase<AstNodePtr>:: SkipUntil( const AstNodePtr& s)
     {  PushScope( s,  BEFORE, BEFORE); }
template <class AstNodePtr>
void ProcessAstTreeBase<AstNodePtr>:: SkipOnly( const AstNodePtr& s)
     {
        PushScope( s,  AT, BEFORE);
     }
template <class AstNodePtr>
bool ProcessAstTreeBase<AstNodePtr>:: operator()( AstInterface &fa, const AstNodePtr& s)
    {
      AstNodePtr tmp = cur;
      AstNodePtrImpl s_(s);
      bool r = ReadAstTraverse(fa, s_.get_ptr(), *this, AstInterface::PreAndPostOrder);
      cur = tmp;
      return r;
    }

template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
                       AstInterface::TraversalVisitType t)
     {
        if (t == AstInterface::PreVisit) {
           Skip(s);
           SkipUntil( body);
        }
        return true;
     }
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& /*cond*/, const AstNodePtr& truebody,
                             const AstNodePtr& /*falsebody*/,
                             AstInterface::TraversalVisitType t)
     {
        if (t == AstInterface::PreVisit) {
           SkipUntil( truebody );
        }
       return true;
     }
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>::
ProcessFunctionDefinition(AstInterface&, const AstNodePtr&, AstInterface::AstList&, AstInterface::AstList&,
                          const AstNodePtr&, AstInterface::TraversalVisitType)
{ 
   return true;
}
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>::
ProcessBlock( AstInterface &, const AstNodePtr&, AstInterface::TraversalVisitType) {
    return true;
}
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: ProcessGoto(AstInterface&, const AstNodePtr& s, const AstNodePtr&) {
    Skip(s);
    return true;
}
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: ProcessDecls(AstInterface &fa, const AstNodePtr& s)
                                { Skip(s); return true; }
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: ProcessStmt(AstInterface &fa, const AstNodePtr& s)
                               { Skip(s); return true; }
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: ProcessTree( AstInterface &_fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
   {
     AstInterface& fa = static_cast<AstInterface&>(_fa);
     AstNodePtr init, cond, incr, body;
     AstInterface::AstList params, outParams;
     bool success = true;
     if (fa.IsFunctionDefinition(s, 0, &params, &outParams, &body)) {
        success = ProcessFunctionDefinition(fa, s, params, outParams, body, t);
     }
     if (fa.IsBlock(s)) {
        success = ProcessBlock( fa, s, t);
     }
     else if (fa.IsLoop(s, 0,0,0,&body)) {
        success = ProcessLoop( fa, s, body, t);
     }
     else if (fa.IsIf(s, &cond, &body, &incr)) {
        success = ProcessIf( fa, s, cond, body, incr, t);
     }
     else if ( t == AstInterface::PreVisit) {
       if (fa.IsGoto(s, &incr))
           success = ProcessGoto(fa, s, incr);
       else if ( fa.IsExecutableStmt(s))
           success = ProcessStmt(fa, s);
       else if ( fa.IsDecls(s))
           success = ProcessDecls(fa, s);
     }
     return success;
   }
template <class AstNodePtr>
bool ProcessAstTree<AstNodePtr>:: operator()( AstInterface &fa, const AstNodePtr& s)
    { return ReadAstTraverse(fa, s, *this, AstInterface::PreAndPostOrder); }
#endif
