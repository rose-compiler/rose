
#ifndef PROCESS_AST_TREE
#define PROCESS_AST_TREE

#include "AstInterface.h"
#include "SinglyLinkedList.h"

class ProcessAstTreeBase : public ProcessAstNode
{
 public:
  typedef enum {BEFORE = 1, AT = 2, INSIDE = 4, BEFORE_AT = 3, INSIDE_AT = 6} TraverseLocation;
 private:
  struct TraverseState{ 
      AstNodePtr scope; 
      int state;  
      int skip;
      TraverseState( const AstNodePtr &_scope, int _skip, 
                     TraverseLocation _state)
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
  //! Inherited from ProcessAstNode
  bool Traverse( AstInterface &fa, const AstNodePtr& s, 
                      AstInterface::TraversalVisitType t);
 protected:
  virtual bool ProcessTree( AstInterface &fa, const AstNodePtr& s,
                               AstInterface::TraversalVisitType t) = 0;

  void Skip( const AstNodePtr& s) ;
  void SkipUntil( const AstNodePtr& s);
  void SkipOnly( const AstNodePtr& s);
 public:
  //! Read only traverse
  bool operator()( AstInterface &fa, const AstNodePtr& s);

// DQ (2/18/2008): Added to fix warning from GNU g++
   virtual ~ProcessAstTreeBase() {}      
};


class ProcessAstTree : public ProcessAstTreeBase
{
 protected:
  virtual bool ProcessLoop(AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& body,
                               AstInterface::TraversalVisitType t) ;
  virtual bool ProcessIf( AstInterface &fa, const AstNodePtr& s,
                             const AstNodePtr& cond, const AstNodePtr& truebody,
                             const AstNodePtr& falsebody, 
                             AstInterface::TraversalVisitType t) ;
  virtual bool ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                                              const AstNodePtr& body,
                                             AstInterface::TraversalVisitType t) ;
  virtual bool ProcessBlock( AstInterface &fa, const AstNodePtr& s,
                                     AstInterface::TraversalVisitType t);
  virtual bool ProcessGoto( AstInterface &fa, const AstNodePtr& s, 
                               const AstNodePtr& dest);
  virtual bool ProcessDecls(AstInterface &fa, const AstNodePtr& s);
  virtual bool ProcessStmt(AstInterface &fa, const AstNodePtr& s);
  bool ProcessTree( AstInterface &_fa, const AstNodePtr& s, 
                       AstInterface::TraversalVisitType t);
 public:
  bool operator()( AstInterface &fa, const AstNodePtr& s);
};

#endif
