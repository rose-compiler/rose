#ifndef ASTBIN_TRAVERSAL_H
#define ASTBIN_TRAVERSAL_H

#include "RoseBin_support.h"

class AST_BIN_Traversal
  : public AstSimpleProcessing {
  // Checker specific parameters should be allocated here.
    std::ofstream myfile;
    RoseBin_unparse_visitor* unparser;
    int nrOfInstructions;
public:
    int getNrOfInstructions() {return nrOfInstructions;}
    AST_BIN_Traversal() {}
    ~AST_BIN_Traversal() {}
  // The implementation of the run function has to match the traversal being called.
  void run(SgNode* n, std::string file){ 
  // create file
    nrOfInstructions=0;
    unparser = RoseBin_support::getUnparseVisitor();
    myfile.open(file.c_str());
    myfile << "digraph \"" << file << "\" {\n" << std::endl;
    this->traverse(n, preorder); 
    myfile << "}\n" << std::endl;
    myfile.close();
  };
  void visit(SgNode* n) {
    if (n) {
      nrOfInstructions++;
      std::string name = "";
      if (isSgAsmInstruction(n))
	name = unparser->unparseInstruction(isSgAsmInstruction(n));
      SgNode* parent = n->get_parent();
      // node
      std::string add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmFunctionDeclaration(n)) { 
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
	name = isSgAsmFunctionDeclaration(n)->get_name();
      }
      if (isSgAsmx86Call(n))
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmValueExpression(n))
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmMemoryReferenceExpression(n))
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmBinaryExpression(n))
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmRegisterReferenceExpression(n)) {
	std::string exprName = unparser->resolveRegister(isSgAsmRegisterReferenceExpression(n)->get_x86_register_code (), 
						    isSgAsmRegisterReferenceExpression(n)->get_x86_position_in_register_code ());
	name += " "+exprName;
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
      }
      if (isSgAsmOperandList(n))
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=white,fontname=\"7x13bold\",fontcolor=black,style=filled";
      myfile << "\"" << n << "\"[label=\"" << name << "\\n" << n->class_name() << "\"" << add << " ];\n"; 
      if (parent) {
	// edge
	myfile << "\"" << parent << "\" -> \"" << n << "\" [label=\"" << n->class_name() << "\" ];\n"; 
      }
    }
  }
  
};



#endif
