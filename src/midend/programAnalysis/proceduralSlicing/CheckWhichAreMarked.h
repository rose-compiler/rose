
#ifndef CheckMark_IS_INCLUDED
#define CheckMark_IS_INCLUDED


/*!
  This class is only for checking which AST nodes have and don't have the astAttribute "keep". This class is not necessary for the slicing process.
*/

class CheckWhichAreMarked : public AstSimpleProcessing{
  
 protected:
  /*!
    \brief This visit function checks the astNode node if it has or does not have the AstAttribute keep
  */
  void virtual visit(SgNode* node){
    if(node->attribute.exists("keep")){
      cout <<"Marked to be kept: "<< node->sage_class_name() <<": " << node->unparseToString() << endl;
    }
    else{
      cout <<"NOT marked to be kept: "<<node->sage_class_name() <<": "<< endl;//node->unparseToString()<<endl;
    }
  }

};

#endif
