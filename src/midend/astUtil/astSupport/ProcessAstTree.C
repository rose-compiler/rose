// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include "ProcessAstTree.h"

void ProcessAstTreeBase:: SetLocation( TraverseLocation state)
   { 
     scopeStack.First()->GetEntry().state = state;
   }
ProcessAstTreeBase::TraverseState ProcessAstTreeBase:: GetScope() const
   { 
     return (scopeStack.size() > 0)? scopeStack.First()->GetEntry()
                     : TraverseState(AstNodePtr(), 0, BEFORE); 
   }
void ProcessAstTreeBase:: PopScope()
   {
     scopeStack.PopFirst();
   }
void ProcessAstTreeBase:: 
PushScope( const AstNodePtr& scope, int skip, TraverseLocation state)
   { 
     TraverseState tmp(scope, skip, state);
     scopeStack.PushFirst(tmp);
   }
bool ProcessAstTreeBase:: Traverse( AstInterface &fa, const AstNodePtr& s, 
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
void ProcessAstTreeBase:: Skip( const AstNodePtr& s) 
     {  
        if (cur == s)  {
            PushScope(s, INSIDE, INSIDE); 
        }
        else {
            PushScope(s, INSIDE_AT, BEFORE);
        }
     }
void ProcessAstTreeBase:: SkipUntil( const AstNodePtr& s)
     {  PushScope( s,  BEFORE, BEFORE); }
void ProcessAstTreeBase:: SkipOnly( const AstNodePtr& s)
     {
        PushScope( s,  AT, BEFORE);
     }
bool ProcessAstTreeBase:: operator()( AstInterface &fa, const AstNodePtr& s)
    { 
      AstNodePtr tmp = cur;
      bool r = ReadAstTraverse(fa, s, *this, AstInterface::PreAndPostOrder); 
      cur = tmp;
      return r;
    }
//-----------------ProcessAstTree -------------------
bool ProcessAstTree:: ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
                               AstInterface::TraversalVisitType t) 
     { 
        if (t == AstInterface::PreVisit) {
           Skip(s);
           SkipUntil( body);
        }
        return true; 
     }
bool ProcessAstTree:: ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& cond, const AstNodePtr& truebody,
                             const AstNodePtr& falsebody, 
                             AstInterface::TraversalVisitType t) 
     { 
        if (t == AstInterface::PreVisit) {
           SkipUntil( truebody );
        }
       return true; 
     }
bool ProcessAstTree:: 
ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                         const AstNodePtr& body, AstInterface::TraversalVisitType t) 
          { return true;}
bool ProcessAstTree:: 
ProcessBlock( AstInterface &fa, const AstNodePtr& s,
                                  AstInterface::TraversalVisitType t) 
  { return true;}
bool ProcessAstTree:: ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& dest)
                                { Skip(s); return true; }
bool ProcessAstTree:: ProcessDecls(AstInterface &fa, const AstNodePtr& s)
                                { Skip(s); return true; }
bool ProcessAstTree:: ProcessStmt(AstInterface &fa, const AstNodePtr& s)
                               { Skip(s); return true; }
bool ProcessAstTree:: ProcessTree( AstInterface &_fa, const AstNodePtr& s, 
                       AstInterface::TraversalVisitType t)
   {
     AstInterface& fa = static_cast<AstInterface&>(_fa);
     AstNodePtr init, cond, incr, body;
     bool success = true;
     if (fa.IsFunctionDefinition(s, 0, 0, 0, &body)) {
        success = ProcessFunctionDefinition(fa, s, body, t);
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
bool ProcessAstTree:: operator()( AstInterface &fa, const AstNodePtr& s)
    { return ReadAstTraverse(fa, s, *this, AstInterface::PreAndPostOrder); }

