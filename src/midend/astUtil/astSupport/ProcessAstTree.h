
#ifndef PROCESS_AST_TREE
#define PROCESS_AST_TREE

#include <AstInterface.h>
#include <SinglyLinkedList.h>

class ProcessAstTreeBase : public ProcessAstNode
{
 public:
  typedef enum {BEFORE = 1, AT = 2, INSIDE = 4, BEFORE_AT = 3, INSIDE_AT = 6} TraverseLocation;
 private:
  struct TraverseState{
      AstNodePtr scope;
      int state;
      int skip;

      TraverseState( const AstNodePtr &_scope, int _skip, TraverseLocation _state)
                    : scope(_scope), state(_state), skip(_skip) {}
      TraverseState( const TraverseState& that)
                    : scope(that.scope), state(that.state), skip(that.skip) {}
      void operator = ( const TraverseState& that)
         { scope = that.scope; skip = that.skip; state = that.state; }
  };
  SinglyLinkedListWrap<TraverseState> scopeStack;
  AstNodePtr cur;


  void SetLocation( TraverseLocation state);
  TraverseState GetScope() const;
  void PopScope();
  void PushScope( const AstNodePtr& scope, int skip, TraverseLocation state);
//Boolean Traverse( AstInterface &fa, const AstNodePtr& s, 
  int Traverse( AstInterface &fa, const AstNodePtr& s, 
                      AstInterface::TraversalVisitType t);
 protected:
//virtual Boolean ProcessTree( AstInterface &fa, const AstNodePtr& s,
  virtual int ProcessTree( AstInterface &fa, const AstNodePtr& s,
                               AstInterface::TraversalVisitType t) = 0;

  void Skip( const AstNodePtr& s) ;
  void SkipUntil( const AstNodePtr& s);
  void SkipOnly( const AstNodePtr& s);
 public:
//Boolean operator()( AstInterface &fa, const AstNodePtr& s);
  int operator()( AstInterface &fa, const AstNodePtr& s);
};


class ProcessAstTree : public ProcessAstTreeBase
{
 protected:
//virtual Boolean ProcessLoop(AstInterface &fa, const AstNodePtr& s, 
  virtual int ProcessLoop(AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& body,
	                       AstInterface::TraversalVisitType t) ;
//virtual Boolean ProcessIf( AstInterface &fa, const AstNodePtr& s,
  virtual void ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& cond, const AstNodePtr& truebody,
                             const AstNodePtr& falsebody, 
                             AstInterface::TraversalVisitType t) ;
//virtual Boolean ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
  virtual int ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                                              const AstNodePtr& body,
                                             AstInterface::TraversalVisitType t) ;
//virtual Boolean ProcessBasicBlock( AstInterface &fa, const AstNodePtr& s,
  virtual int ProcessBasicBlock( AstInterface &fa, const AstNodePtr& s,
                                     AstInterface::TraversalVisitType t);
//virtual Boolean ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
  virtual int ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& dest);
//virtual Boolean ProcessDecls(AstInterface &fa, const AstNodePtr& s);
  virtual int ProcessDecls(AstInterface &fa, const AstNodePtr& s);
//virtual Boolean ProcessStmt(AstInterface &fa, const AstNodePtr& s);
  virtual void ProcessStmt(AstInterface &fa, const AstNodePtr& s);
//Boolean ProcessTree( AstInterface &_fa, const AstNodePtr& s, 
  int ProcessTree( AstInterface &_fa, const AstNodePtr& s, 
                       AstInterface::TraversalVisitType t);
 public:
//Boolean operator()( AstInterface &fa, const AstNodePtr& s);
  int operator()( AstInterface &fa, const AstNodePtr& s);
};

#endif
