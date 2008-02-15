#include <general.h>
#include <iostream>
#include <ProcessAstTree.h>

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

void ProcessAstTreeBase:: SetLocation( TraverseLocation state)
   { 
     scopeStack.First()->GetEntry().state = state;
   }
ProcessAstTreeBase::TraverseState ProcessAstTreeBase:: GetScope() const
   { 
     return (scopeStack.size() > 0)? scopeStack.First()->GetEntry()
                     : TraverseState(0, 0, BEFORE); 
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
Boolean ProcessAstTreeBase:: Traverse( AstInterface &fa, const AstNodePtr& s, 
                      AstInterface::TraversalVisitType t)
   {
     cur = s;
     Boolean pass = true, result = true;
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
     else if (state.scope != 0 && (state.skip & state.state) )
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
Boolean ProcessAstTreeBase:: operator()( AstInterface &fa, const AstNodePtr& s)
    { 
      AstNodePtr tmp = cur;
      Boolean r = ReadAstTraverse(fa, s, *this, AstInterface::PreAndPostOrder); 
      cur = tmp;
      return r;
    }

Boolean ProcessAstTree:: ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
	                       AstInterface::TraversalVisitType t) 
     { 
        if (t == AstInterface::PreVisit) {
           Skip(s);
           SkipUntil( body);
        }
        return true; 
     }
void ProcessAstTree:: ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& cond, const AstNodePtr& truebody,
                             const AstNodePtr& falsebody, 
                             AstInterface::TraversalVisitType t) 
     { 
        if (t == AstInterface::PreVisit) {
           SkipUntil( truebody );
        }
     }
Boolean ProcessAstTree:: 
ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                         const AstNodePtr& body, AstInterface::TraversalVisitType t) 
          { return true;}
Boolean ProcessAstTree:: 
ProcessBasicBlock( AstInterface &fa, const AstNodePtr& s,
                                  AstInterface::TraversalVisitType t) 
  { return true;}
Boolean ProcessAstTree:: ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& dest)
                                { Skip(s); return true; }
Boolean ProcessAstTree:: ProcessDecls(AstInterface &fa, const AstNodePtr& s)
                                { Skip(s); return true; }
void ProcessAstTree:: ProcessStmt(AstInterface &fa, const AstNodePtr& s)
                               { Skip(s); }
Boolean ProcessAstTree:: ProcessTree( AstInterface &_fa, const AstNodePtr& s, 
                       AstInterface::TraversalVisitType t)
   {
     AstInterface& fa = static_cast<AstInterface&>(_fa);
     AstNodePtr init, cond, incr, body;
     Boolean success = true;
     if (fa.IsFunctionDefinition(s, 0, &body)) {
        success = ProcessFunctionDefinition(fa, s, body, t);
     }
     if (fa.IsBasicBlock(s)) {
        success = ProcessBasicBlock( fa, s, t);
     }
     else if (fa.IsLoop(s, 0,0,0,&body)) {
        success = ProcessLoop( fa, s, body, t);
     }
     else if (fa.IsIf(s, &cond, &body, &incr)) {
        ProcessIf( fa, s, cond, body, incr, t);
     }
     else if ( t == AstInterface::PreVisit) {
       if (fa.IsGoto(s, &incr)) 
           success = ProcessGoto(fa, s, incr);
       else if ( fa.IsExecutableStmt(s)) 
           ProcessStmt(fa, s);
       else if ( fa.IsDecls(s)) 
           success = ProcessDecls(fa, s);
     }
     return success;
   }
Boolean ProcessAstTree:: operator()( AstInterface &fa, const AstNodePtr& s)
    { return ReadAstTraverse(fa, s, *this, AstInterface::PreAndPostOrder); }

