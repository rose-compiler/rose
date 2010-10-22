// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AST_BIN_Traversal.h"


void AST_BIN_Traversal::run(SgNode* n, std::string file) {
  // create file
    nrOfInstructions=0;
    myfile.open(file.c_str());
    myfile << "digraph \"" << file << "\" {\n" << std::endl;
    this->traverse(n, preorder); 
    myfile << "}\n" << std::endl;
    myfile.close();
}

void AST_BIN_Traversal::visit(SgNode* n) {
    if (n) {
      nrOfInstructions++;
      std::string name = "";
      if (isSgAsmInstruction(n))
        name = unparseInstruction(isSgAsmInstruction(n));
      SgNode* parent = n->get_parent();
      // node
      std::string add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmFunctionDeclaration(n)) { 
        add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
        name = isSgAsmFunctionDeclaration(n)->get_name();
      }
      if (isSgAsmx86Instruction(n) && isSgAsmx86Instruction(n)->get_kind() == x86_call)
        add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmValueExpression(n))
        add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmMemoryReferenceExpression(n))
        add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmBinaryExpression(n))
        add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
      if (isSgAsmx86RegisterReferenceExpression(n)) {
        SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(n);
        std::string exprName = unparseX86Register(rr->get_descriptor());
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
