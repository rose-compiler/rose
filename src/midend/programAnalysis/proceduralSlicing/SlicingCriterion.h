#ifndef SlicingCriterion_IS_DEFINED
#define SlicingCriterion_IS_DEFINED



/*!
  \class SlicingCriterion
  
  This class find all the statements between two pragma declarations. These statements are the slicing
  criterion. It might be one or more statements. 
  
  The function "get_stmts_between_pragma" returns these statements in a set.
*/
class SlicingCriterion : public AstSimpleProcessing{
  
 public:
  /*!
    \brief Returns the set of statements which are between the pragma declarations. These statements
    are the slicing criterion.
  */
  set<SgStatement*> get_stmts_between_pragma(){ return stmts; }
  /*!
    \brief Initializes the boolean variables used in the traversal to tell us if we are before the
    first pragma or if we are after the last pragma. Could be in a constructor??
  */
  void set_bool(){ next = false; foundall = false;}
  
 protected:
  void virtual visit(SgNode* astNode);
  
  // private:
  set<SgStatement*> stmts;
  bool next, foundall;
};

#endif
