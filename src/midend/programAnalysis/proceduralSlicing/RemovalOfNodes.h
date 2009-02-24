
#ifndef RemovalOfNodes_IS_INCLUDED
#define RemovalOfNodes_IS_INCLUDED


/*!
  This class removes AST nodes which don't have the astAttribute "keep".
*/

//#define DEBUG_REMOVAL

class RemovalOfNodes : public AstSimpleProcessing{
  
 protected:
  /*!
    \brief This visit function removes the astNode node if it does not have the AstAttribute keep
   */
  void virtual visit(SgNode* node);
  
 private:
  /*!
    This function writes a short message about what kind of AstNode is removed.
  */
  void writeMessage(SgNode* node);
};

#endif
