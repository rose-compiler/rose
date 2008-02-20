/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that actually builds the Rose Tree
 ****************************************************/
#include "rose.h"
#include "RoseBin_IDAPRO_buildTree.h"

using namespace std;

using namespace RoseBin_Def;

SgAsmType* getRegisterType(SgAsmRegisterReferenceExpression::x86_position_in_register_enum regSize) {
	SgAsmType* type = NULL;
	switch(regSize) {
	case SgAsmRegisterReferenceExpression::low_byte : type=SgAsmTypeByte::createType(); break; 
	case SgAsmRegisterReferenceExpression::high_byte : type=SgAsmTypeByte::createType(); break; 
	case SgAsmRegisterReferenceExpression::word : type=SgAsmTypeWord::createType(); break; 
	case SgAsmRegisterReferenceExpression::dword : type=SgAsmTypeDoubleWord::createType(); break; 
	case SgAsmRegisterReferenceExpression::qword : type=SgAsmTypeQuadWord::createType(); break; 
	case SgAsmRegisterReferenceExpression::all : type=SgAsmTypeQuadWord::createType(); break; 
	default:
	  ROSE_ASSERT(false);
	}
	return type;
}

/****************************************************
 * create a BinaryNode
 ****************************************************/
SgAsmExpression* RoseBin_IDAPRO_buildTree::convertBinaryNode(exprTreeType* expt, 
							     list<SgAsmExpression*> *children, 
							     const string& typeOfOperandIn) {
  string typeOfOperand = typeOfOperandIn;
  SgAsmExpression* binNode=NULL;
  int sizeOfList = children->size();
  bool isLeaf = false;
  if (sizeOfList==0)
    isLeaf = true;
  if (RoseBin_support::DEBUG_MODE()) {
    ostringstream addrhex;
    addrhex << hex << setw(8) << expt->immediate ;

    cout << "\n>> convert binNode: expr_id: " << expt->id << endl;
    cout << "   isLeaf: " << RoseBin_support::resBool(isLeaf) << 
      "  symbol: " << expt->symbol << 
      "  immedi: " << expt->immediate << " hex: " << addrhex.str() << endl;
  }

  if (expt->expr_type==4) {
    // this is a operator
    if (RoseBin_support::DEBUG_MODE())
      cout << " its a operator ... " << expt->symbol << endl;

    string symbol = RoseBin_support::str_to_upper(expt->symbol);
    if (symbol=="CS" || symbol=="DS" || symbol=="SS" || symbol=="ES"
        || symbol == "FS" || symbol=="GS") {
      // we need the child information to build this
      if (sizeOfList!=1) {
        cerr << " a reference expression can only take one child! " <<endl;
        list<SgAsmExpression*>::iterator childList = children->begin();
        for (; childList!=children->end();++childList) {
          SgAsmExpression* child = *childList;
          exprTreeType exprTree =  debugHelpMap[child];
          cout << " children are : " << child << " -- " << exprTree.symbol << endl;
        }
        
        exit(0);
      }
      SgAsmExpression* child = *(children->begin());
      ROSE_ASSERT(RoseAssemblyLanguage==x86);
      SgAsmRegisterReferenceExpression::x86_register_enum registerSg = 
	SgAsmRegisterReferenceExpression::undefined_general_register;
      SgAsmRegisterReferenceExpression::x86_position_in_register_enum regSize =  
	SgAsmRegisterReferenceExpression::undefined_position_in_register;
      resolveRegister(symbol, &registerSg, &regSize);
      if (isSgAsmMemoryReferenceExpression(child)) {
        binNode = new SgAsmRegisterReferenceExpression();
	isSgAsmRegisterReferenceExpression(binNode)->set_type(getRegisterType(regSize));
        (isSgAsmRegisterReferenceExpression(binNode))->set_x86_register_code(registerSg);
        (isSgAsmRegisterReferenceExpression(binNode))->set_x86_position_in_register_code(regSize);
	isSgAsmMemoryReferenceExpression(child)->set_segment(binNode);
	binNode->set_parent(child);
	binNode=child;

      } else {
	// do nothing
	// if we have a jump case, we have a value only and no RegRef
      }
#if 0
      // resolve the register information
      if (RoseAssemblyLanguage==x86) {
        SgAsmRegisterReferenceExpression::x86_register_enum registerSg = 
          SgAsmRegisterReferenceExpression::undefined_general_register;
        SgAsmRegisterReferenceExpression::x86_position_in_register_enum regSize =  
          SgAsmRegisterReferenceExpression::undefined_position_in_register;
        resolveRegister(symbol, &registerSg, &regSize);
        //      binNode = new SgAsmRegisterReferenceExpression(registerSg, regSize);
        binNode = new SgAsmRegisterReferenceExpression();
        (isSgAsmRegisterReferenceExpression(binNode))->set_x86_register_code(registerSg);
        (isSgAsmRegisterReferenceExpression(binNode))->set_x86_position_in_register_code(regSize);
      } else if (RoseAssemblyLanguage==arm) {
        SgAsmRegisterReferenceExpression::arm_register_enum registerSg = 
          SgAsmRegisterReferenceExpression::undefined_arm_register;
        SgAsmRegisterReferenceExpression::arm_position_in_register_enum regSize =  
          SgAsmRegisterReferenceExpression::undefined_arm_position_in_register;
        resolveRegister(symbol, &registerSg, &regSize);
        //      binNode = new SgAsmRegisterReferenceExpression(registerSg, regSize);
        binNode = new SgAsmRegisterReferenceExpression();
        (isSgAsmRegisterReferenceExpression(binNode))->set_arm_register_code(registerSg);
        (isSgAsmRegisterReferenceExpression(binNode))->set_arm_position_in_register_code(regSize);
      }
      // the child could be the expression to an address,  e.g. ss:[ebp]
      
      // todo : dont know how to change this right now. Affected by AST structure change
      //isSgAsmRegisterReferenceExpression(binNode)->set_segment(child); 
      rememberOffset = isSgAsmRegisterReferenceExpression(binNode);
      if (child==NULL) {
        cerr << "adding no child to RegisterReference " << endl;
      }
      child->set_parent(binNode);
#endif
    }
    else if (expt->symbol=="+") {
      list<SgAsmExpression*>::iterator childList = children->begin();
      int count=0;
      if (children->size()==1) {
	// the add has only one child
	SgAsmExpression* child = *(children->begin());
	if (child && previousExp && child!=previousExp) {
	  child->set_parent(previousExp);
	  if (isSgAsmMemoryReferenceExpression(previousExp)) 
	    isSgAsmMemoryReferenceExpression(previousExp)->set_address(child);
	  // changed on 16Jan08
	  //if (isSgAsmRegisterReferenceExpression(previousExp))
	  //  isSgAsmRegisterReferenceExpression(previousExp)->set_offset(child);
	}
      } else {
	binNode = new SgAsmBinaryAdd();        
	for (; childList!=children->end();++childList) {
	  SgAsmExpression* child = *childList;
	  if (child) {
	    if (count==0)
	      isSgAsmBinaryAdd(binNode)->set_lhs(child);
	    else
	      isSgAsmBinaryAdd(binNode)->set_rhs(child);
	    child->set_parent(binNode);
	  }
	  count++;
	}
      }
    } 
    else if (expt->symbol=="-") {
      binNode = new SgAsmBinarySubtract();
      list<SgAsmExpression*>::iterator childList = children->begin();
      int count=0;
      for (; childList!=children->end();++childList) {
        SgAsmExpression* child = *childList;
        if (count==0)
          isSgAsmBinarySubtract(binNode)->set_lhs(child);
        else
          isSgAsmBinarySubtract(binNode)->set_rhs(child);
        count++;
        child->set_parent(binNode);
      }
    } 
    else if (expt->symbol=="*") {
      binNode = new SgAsmBinaryMultiply();
      list<SgAsmExpression*>::iterator childList = children->begin();
      int count=0;
      for (; childList!=children->end();++childList) {
        SgAsmExpression* child = *childList;
        if (count==0)
          isSgAsmBinaryMultiply(binNode)->set_lhs(child);
        else
          isSgAsmBinaryMultiply(binNode)->set_rhs(child);
        count++;
        child->set_parent(binNode);
      }
    } 
    else if (expt->symbol=="/") {
      binNode = new SgAsmBinaryDivide();
      list<SgAsmExpression*>::iterator childList = children->begin();
      int count=0;
      for (; childList!=children->end();++childList) {
        SgAsmExpression* child = *childList;
        if (count==0)
          isSgAsmBinaryDivide(binNode)->set_lhs(child);
        else
          isSgAsmBinaryDivide(binNode)->set_rhs(child);
        count++;
        child->set_parent(binNode);
      }
    } 
    else if (expt->symbol=="[") {
      // the child is the expression that constitutes the address , e.g. [ebp]
      SgAsmExpression* child = *(children->begin());
      // the child is another expression, like +, - , ...
      ROSE_ASSERT(child);
      binNode = new SgAsmMemoryReferenceExpression();
      isSgAsmMemoryReferenceExpression(binNode)->set_type(SgAsmTypeQuadWord::createType());
      ROSE_ASSERT (binNode->get_type());

      isSgAsmMemoryReferenceExpression(binNode)->set_address(child);
      child->set_parent(binNode);
      //      isSgAsmMemoryReferenceExpression(binNode)->set_offset(rememberOffset);
      //rememberOffset->set_parent(binNode);
      //rememberOffset=NULL;
    } 
    
    else if (expt->symbol=="b4" || expt->symbol=="b2" || expt->symbol=="b1" || expt->symbol=="b6" || expt->symbol=="b8") {
      // since b4, b2, b1 are types and no nodes,
      // we return the binNode of the child
      binNode = *(children->begin());
      if (isSgAsmMemoryReferenceExpression(binNode)) {
	SgAsmMemoryReferenceExpression* memRefT = isSgAsmMemoryReferenceExpression(binNode);
	if (expt->symbol=="b1") 
	  memRefT->set_type(SgAsmTypeByte::createType());
	else if (expt->symbol=="b2") 
	  memRefT->set_type(SgAsmTypeWord::createType());
	else if (expt->symbol=="b4") 
	  memRefT->set_type(SgAsmTypeDoubleWord::createType());
	else if (expt->symbol=="b6") 
	  ROSE_ASSERT(false);
	else if (expt->symbol=="b8") 
	  memRefT->set_type(SgAsmTypeQuadWord::createType());
	ROSE_ASSERT (memRefT->get_type());
      } 
    }

    else {
      cerr << "ERROR:: FIXME:: symbol not resolved " << expt->symbol << endl;
      // temp solution for arm. tps (09/17/07)
      binNode = new SgAsmByteValueExpression();
      isSgAsmByteValueExpression(binNode)->set_value('5'); // ascii for 5

      //      exit(0);
    }
  } 

  else if (expt->expr_type==2) {
    // its a value
    if (RoseBin_support::DEBUG_MODE())
      cout << " its a value... resolving type: --- " << typeOfOperand << " --- " ;
    // fixme .. temporary fix for DB issue
    if (typeOfOperand=="BYTE") typeOfOperand="WORD";
    if (typeOfOperand=="WORD") typeOfOperand="DWORD";

    if (typeOfOperand=="BYTE") {
      binNode = new SgAsmByteValueExpression();
      isSgAsmByteValueExpression(binNode)->set_value(expt->immediate);
    } else 
      if (typeOfOperand=="WORD") {
        binNode = new SgAsmWordValueExpression();
        isSgAsmWordValueExpression(binNode)->set_value(expt->immediate);
      } else 
        if (typeOfOperand=="DWORD") {
          binNode = new SgAsmDoubleWordValueExpression();
          isSgAsmDoubleWordValueExpression(binNode)->set_value(expt->immediate);
        } else
          if (typeOfOperand=="QWORD") {
            binNode = new SgAsmQuadWordValueExpression();
            isSgAsmQuadWordValueExpression(binNode)->set_value(expt->immediate);
          } else 
            if (typeOfOperand=="SFLOAT") {
              binNode = new SgAsmSingleFloatValueExpression();
              isSgAsmQuadWordValueExpression(binNode)->set_value(expt->immediate);
            } else 
              if (typeOfOperand=="DFLOAT") {
                binNode = new SgAsmDoubleFloatValueExpression();
                isSgAsmQuadWordValueExpression(binNode)->set_value(expt->immediate);
              } else {
                cerr << "ERROR :: unhandled type of value: " << typeOfOperand << " val: " << 
                  RoseBin_support::ToString(expt->immediate) << endl;
		//              exit(0);
		// creating defualt for now
		binNode = new SgAsmDoubleWordValueExpression();
		isSgAsmDoubleWordValueExpression(binNode)->set_value(expt->immediate);
	      }
    
    if (RoseBin_support::DEBUG_MODE())
      cout <<  typeOfOperand << endl;
    //printExprNode(*expt);
  }
 
  else if (expt->expr_type==1) {
    // register
    if (RoseBin_support::DEBUG_MODE())
      cout << " its a register .... " << endl;

    if (RoseAssemblyLanguage==x86) {
      SgAsmRegisterReferenceExpression::x86_register_enum registerSg = 
        SgAsmRegisterReferenceExpression::undefined_general_register;
      SgAsmRegisterReferenceExpression::x86_position_in_register_enum regSize =  
        SgAsmRegisterReferenceExpression::undefined_position_in_register;
      string symbol = RoseBin_support::str_to_upper(expt->symbol);
      
      resolveRegister(symbol, &registerSg, &regSize);
      //binNode = new SgAsmRegisterReferenceExpression(registerSg, regSize);
      binNode = new SgAsmRegisterReferenceExpression();
      isSgAsmRegisterReferenceExpression(binNode)->set_type(getRegisterType(regSize));
      (isSgAsmRegisterReferenceExpression(binNode))->set_x86_register_code(registerSg);
      (isSgAsmRegisterReferenceExpression(binNode))->set_x86_position_in_register_code(regSize);
    } else if (RoseAssemblyLanguage==arm) {
      SgAsmRegisterReferenceExpression::arm_register_enum registerSg = 
	SgAsmRegisterReferenceExpression::undefined_arm_register;
      SgAsmRegisterReferenceExpression::arm_position_in_register_enum regSize =  
	SgAsmRegisterReferenceExpression::undefined_arm_position_in_register;
      string symbol = RoseBin_support::str_to_upper(expt->symbol);
      
      resolveRegister(symbol, &registerSg, &regSize);
      //      binNode = new SgAsmRegisterReferenceExpression(registerSg, regSize);
      binNode = new SgAsmRegisterReferenceExpression();
      // todo : find out types for ARM
      (isSgAsmRegisterReferenceExpression(binNode))->set_arm_register_code(registerSg);
      (isSgAsmRegisterReferenceExpression(binNode))->set_arm_position_in_register_code(regSize);
    }
  } else {
    cerr << " ERROR ... buildTree ... wrong type " << endl;
    RoseBin_support::printExprNode(*expt);
    exit(0);
  }
  return binNode;
}

/****************************************************
 * return the replacement text for an expression
 ****************************************************/
string RoseBin_IDAPRO_buildTree::getReplacementText(
						    __gnu_cxx::hash_map< int, exprSubstitutionType> *rememberSubstitution, 
						    int operand_id, 
						    int expr_id_root,
						    int address) {
  string text = "";

  hash_map< int, exprSubstitutionType>::iterator it =  rememberSubstitution->find(address);
  if (it != rememberSubstitution->end()) {
    // for (it; it!=rememberSubstitution->end();++it) {
    const exprSubstitutionType& subst = it->second;
    int op_id = subst.operand_id;
    int ex_id = subst.expr_id;


    if (op_id==operand_id && ex_id==expr_id_root) {
      text=subst.replacement;
      // break;
    }
  }
  return text;
}


/****************************************************
 * return the comment for an expression (address)
 ****************************************************/
std::string RoseBin_IDAPRO_buildTree::getComment(int address, 
                                                 __gnu_cxx::hash_map< int, std::string> *rememberComments) {
  string text = "";

  hash_map< int, string>::iterator it =  rememberComments->find(address);
  if (it != rememberComments->end()) {
    // for (it; it!=rememberComments->end();++it) {
    text = it->second;
  }

  return text;
}

/****************************************************
 * recursive algorithm
 * to create the subtree of one operand
 ****************************************************/
SgAsmExpression* RoseBin_IDAPRO_buildTree::resolveRecursivelyExpression(int address,
									int expr_id_root, 
									const multimap <int,int>& subTree, 
									const string& typeOfOperand,
									std::vector < exprTreeType > *rememberExpressionTree,
									int operand_id,
									__gnu_cxx::hash_map <int, exprSubstitutionType> *rememberSubstitution,
									__gnu_cxx::hash_map <int, string> *rememberComments) {
  SgAsmExpression* binExp=NULL;
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>>>> resolving type of expr_id_root " << expr_id_root << endl;
  list<SgAsmExpression*> children;
  int nrOfChildren=0;

  // collect an array of children
  multimap<int,int>::const_iterator childIt  = subTree.lower_bound(expr_id_root);  
  for (; childIt!=subTree.upper_bound(expr_id_root)  ; ++childIt) {
    // iterate though children of root node
    int child_id = childIt->second;
    int par_id = childIt->first;
    // if (par_id==expr_id_root) {
    if (RoseBin_support::DEBUG_MODE())
      cout << " expr_id: " << expr_id_root << "  child_id: " << child_id << "  itChild : " << nrOfChildren 
	   << " parent_id: " << par_id <<endl;
    ROSE_ASSERT (child_id < (int)rememberExpressionTree->size());
    exprTreeType exprTree = (*rememberExpressionTree)[child_id];
    binExp = resolveRecursivelyExpression(address, child_id, subTree, 
					  typeOfOperand,
					  rememberExpressionTree,
					  operand_id,
					  rememberSubstitution,
					  rememberComments);
    children.push_back(binExp);
    debugHelpMap[binExp] = exprTree; 
    nrOfChildren++;
  }
  if (RoseBin_support::DEBUG_MODE()) {
    cout << "\n>>>> type resolved of expr_id_root " << expr_id_root ;
    cout << "  childCount : " << nrOfChildren ;
  }

  ROSE_ASSERT (expr_id_root < (int)rememberExpressionTree->size());
  exprTreeType exprTree = (*rememberExpressionTree)[expr_id_root];
  binExp = convertBinaryNode(&exprTree, &children, typeOfOperand);
  // the following makes sure that if a node should be skipped
  // e.g. a SgAsmAdd because it has only one child, then
  // we attach to the previous node
  if (binExp!=NULL)
    previousExp = binExp;
  else
    binExp = previousExp;

  // in addition to having the right expression, we 
  // want to add information about the substitution replacement text
  string text = getReplacementText(rememberSubstitution, operand_id, expr_id_root, address);
  string comment = getComment(address, rememberComments);
  if (comment!="") {
    text = text + " <"+comment+">";
  }
  if (binExp!=NULL) {
    string tt = binExp->get_replacement();
    if (tt!="") text = tt;
    binExp->set_replacement(text);
  }

  if (RoseBin_support::DEBUG_MODE())
    cout << ">>>> binExp = " << binExp <<  endl;
  return binExp;
}

