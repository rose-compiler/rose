#include "RoseRDAnalysis.h"

using namespace std;
using namespace CodeThorn;

// obsolete function
void Experimental::RoseRDAnalysis::generateRoseRDDotFile(VariableRenaming* varRen,string filename) {
  ofstream myfile;
  myfile.open(filename.c_str());
  myfile<<"digraph RD1 {"<<endl;
  std::cout << "Propagated Def Table:" << endl;
  VariableRenaming::DefUseTable& defTable=varRen->getPropDefTable();
    BOOST_FOREACH(VariableRenaming::DefUseTable::value_type& node, defTable)
    {
      SgNode* astNode=node.first;
      std::cout << "  Def Table for [" << node.first->class_name() << ":" << astNode << "]:"<< astNode->unparseToString() << std::endl;
      myfile<<"N"<<astNode<<"[label=\""<<astNode->unparseToString()<<"\"];"<<endl;
        BOOST_FOREACH(VariableRenaming::TableEntry::value_type& entry, defTable[astNode])
        {
            std::cout << "    Defs for [" << varRen->keyToString(entry.first) << "]:" << std::endl;
            BOOST_FOREACH(VariableRenaming::NodeVec::value_type& iter, entry.second)
            {
                std::cout << "      -[" << iter->class_name() << ":" << iter << "]" << std::endl;
                std::vector<SgInitializedName*> defNameVec=entry.first;
                ROSE_ASSERT(defNameVec.size()==1);
                myfile<<"N"<<astNode<<"->"<<"N"<<iter<<"[label=\""<<varRen->keyToString(entry.first)<<"\"];"<<endl;
            }
        }
    }
    myfile<<"}"<<endl;
    myfile.close();
}

SgNode* Experimental::RoseRDAnalysis::normalizeAstPointer(SgNode* node) {
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);
  if(SgInitializedName* initName=isSgInitializedName(node))
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(initName->get_parent()))
      node=varDecl;
  return node;
}
SgNode* Experimental::RoseRDAnalysis::normalizeAstPointer2(SgNode* node) {
  SgNode* node0=node;
  while(1) {
    node=normalizeAstPointer(node);
    if(isSgReturnStmt(node)||isSgCastExp(node))
      node=SgNodeHelper::getFirstChild(node);
    if(node0==node)
      break;
    else
      node0=node;
  }
  return node;
}

// errors in rose-RD analysis:
// 1) int a; int b=a; // b=a is not detected as initialization (because a has no def, but better would bespecial init (e.g. '?') for a)
//    this loses information that a variable may not be initialized
// 2) global variables are not represented properly (instead pointer to SgFunctionDefinition is stored)
void Experimental::RoseRDAnalysis::generateRoseRDDotFile2(Labeler* labeler, VariableRenaming* varRen,string filename) {
  ofstream myfile;
  myfile.open(filename.c_str());
  myfile<<"digraph RD1 {"<<endl;

  for(Labeler::iterator i=labeler->begin();i!=labeler->end();++i) {
    SgNode* node=labeler->getNode(*i);
    node=normalizeAstPointer2(node);
    if(!isSgFunctionDeclaration(node)&&!isSgFunctionDefinition(node)&&!isSgBasicBlock(node)&&!isSgWhileStmt(node)&&!isSgForStatement(node)) {
      myfile<<"// ---------------------------------------------------------"<<endl;
      myfile<<"N"<<node<<"[label=\""<<node->class_name()<<"::"<<node->unparseToString()<<"\"];"<<endl;
      VariableRenaming::NumNodeRenameTable useTable=varRen->getUsesAtNode(node);
      for(VariableRenaming::NumNodeRenameTable::iterator j=useTable.begin();j!=useTable.end();++j) {
        VariableRenaming::VarName varName=(*j).first;
        VariableRenaming::NumNodeRenameEntry numNodeRenameEntry=(*j).second;
        for(VariableRenaming::NumNodeRenameEntry::iterator k=numNodeRenameEntry.begin();k!=numNodeRenameEntry.end();++k) {
          int renNum=(*k).first;
          SgNode* renNode=(*k).second;
          renNode=normalizeAstPointer2(renNode); // required to get the variable declaration instead of only the initialized name
          myfile<<"N"<<node<<" -> "<<"N"<<renNode<<"[label=\""<<varRen->keyToString(varName)<<"-"<<renNum <<"\"];"<<endl;
          // the following name label is only required when a non-labeled node is referenced by the RD analysis
          myfile<<"N"<<renNode<<"[label=\""<<renNode->class_name()<<"::"<<renNode->unparseToString()<<"\"];"<<endl;
        }
      }
    }
  }
#if 0
  std::cout << "Propagated Def Table:" << endl;
  VariableRenaming::DefUseTable& defTable=varRen->getPropDefTable();
    BOOST_FOREACH(VariableRenaming::DefUseTable::value_type& node, defTable)
    {
      SgNode* astNode=node.first;
      std::cout << "  Def Table for [" << node.first->class_name() << ":" << astNode << "]:"<< astNode->unparseToString() << std::endl;
      myfile<<"N"<<astNode<<"[label=\""<<astNode->unparseToString()<<"\"];"<<endl;
        BOOST_FOREACH(VariableRenaming::TableEntry::value_type& entry, defTable[astNode])
        {
            std::cout << "    Defs for [" << varRen->keyToString(entry.first) << "]:" << std::endl;
            BOOST_FOREACH(VariableRenaming::NodeVec::value_type& iter, entry.second)
            {
                std::cout << "      -[" << iter->class_name() << ":" << iter << "]" << std::endl;
                std::vector<SgInitializedName*> defNameVec=entry.first;
                ROSE_ASSERT(defNameVec.size()==1);
                myfile<<"N"<<astNode<<"->"<<"N"<<iter<<"[label=\""<<varRen->keyToString(entry.first)<<"\"];"<<endl;
            }
        }
    }
#endif
    myfile<<"}"<<endl;
    myfile.close();
}

void Experimental::RoseRDAnalysis::generateRoseRDDotFiles(Labeler* labeler, SgProject* root) {
      cout << "INFO: generating rose-rd1.dot file."<<endl;
      VariableRenaming varRen(root);
      varRen.run();
      varRen.toFilteredDOT("rose-rd1.dot");
      //varRen.printOriginalDefTable();
      //varRen.printRenameTable();
      cout << "INFO: generating rose-rd2.dot file."<<endl;
      generateRoseRDDotFile2(labeler,&varRen,"rose-rd2.dot");
}
