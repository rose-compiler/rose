//status: works for x = 5, and for x = y + z
//Does not work for right hand side of x = y
//not tested for expression list

// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

// Global variables so that the global function declaration can be reused to build
// each function call expression in the AST traversal to instrument all functions.

class InstrumentationFunction {
private:
   SgFunctionType* type;
   SgFunctionSymbol* symbol;
   string name;
public:
   InstrumentationFunction(const string& name_);
   SgFunctionCallExp* getCallExp() const;
   void buildDeclaration(SgProject* project);
};

InstrumentationFunction::InstrumentationFunction(const string& name_)
   : type(NULL), symbol(NULL), name(name_) {}

SgFunctionCallExp *InstrumentationFunction::getCallExp() const {
   assert(type);
   assert(symbol);
   Sg_File_Info *file_info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
   SgFunctionRefExp* functionRefExpression = new SgFunctionRefExp(new Sg_File_Info(*file_info),
                                                            symbol, type);
   SgExprListExp* expressionList = new SgExprListExp(new Sg_File_Info(*file_info));
   SgFunctionCallExp* functionCallExp = new SgFunctionCallExp(file_info, functionRefExpression,
                                                               expressionList, type);
   assert (functionCallExp != NULL);
   return functionCallExp;
}

void InstrumentationFunction::buildDeclaration(SgProject* project) {
   // *****************************************************
   // Create the functionDeclaration
   // *****************************************************

   Sg_File_Info * file_info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
   SgType *function_return_type = new SgTypeVoid();

   type = new SgFunctionType(function_return_type, false);

   SgFunctionDeclaration *functionDeclaration = new SgFunctionDeclaration(file_info, name, type);

   // ********************************************************************
   // Create the InitializedName for a parameter within the parameter list
   // ********************************************************************
   /*
   SgName var1_name = "textString";

   SgTypeChar * var1_type            = new SgTypeChar();
   SgPointerType *pointer_type       = new SgPointerType(var1_type);
   SgInitializer * var1_initializer  = NULL;
   SgInitializedName *var1_init_name = new SgInitializedName(var1_name, pointer_type, var1_initializer, NULL);

   // Insert argument in function parameter list
   ROSE_ASSERT(functionDeclaration != NULL);
   ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);

   ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
   functionDeclaration->get_parameterList()->append_arg(var1_init_name);
   */
   SgSourceFile* sourceFile = isSgSourceFile(project->get_fileList()[0]);
   ROSE_ASSERT(sourceFile != NULL);
   SgGlobal* globalScope = sourceFile->get_globalScope();
   ROSE_ASSERT(globalScope != NULL);

   // Set the parent node in the AST (this could be done by the AstPostProcessing
   functionDeclaration->set_parent(globalScope);

   // Set the scope explicitly (since it could be different from the parent?)
   // This can't be done by the AstPostProcessing (unless we relax some constraints)
   functionDeclaration->set_scope(globalScope);

   // If it is not a forward declaration then the unparser will skip the ";" at the end (need to fix this better)
   functionDeclaration->setForward();
   ROSE_ASSERT(functionDeclaration->isForward() == true);

   // Mark function as extern "C"
   functionDeclaration->get_declarationModifier().get_storageModifier().setExtern();
   functionDeclaration->set_linkage("C");  // This mechanism could be improved!

   // Add function declaration to global scope
   globalScope->prepend_declaration(functionDeclaration);

   symbol = new SgFunctionSymbol(functionDeclaration);
   // All any modifications to be fixed up (parents etc)
   // AstPostProcessing(project); // This is not allowed and should be fixed!
   AstPostProcessing(globalScope);
}

InstrumentationFunction beforeWrite ("beforeWrite");
InstrumentationFunction beforeRead ("beforeRead");

class InheritedAttribute
   {
     public:
          InheritedAttribute () {};
          InheritedAttribute ( const InheritedAttribute & X ) {};
   };


void instrumentWrite(SgVarRefExp *varRef) {
   SgAssignOp *parent = isSgAssignOp(varRef->get_parent());
   assert(parent != NULL);
   Sg_File_Info * file_info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
   SgCommaOpExp *commaOp = new SgCommaOpExp(file_info, beforeWrite.getCallExp(), varRef,
                                                                           varRef->get_type());
   parent->set_lhs_operand(commaOp);
}

void instrumentRead(SgVarRefExp *varRef) {
   SgExpression *parent = isSgExpression(varRef->get_parent());
   assert(parent != NULL);
   Sg_File_Info * file_info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

   SgCommaOpExp *commaOp = new SgCommaOpExp(file_info, beforeRead.getCallExp(), varRef,
                                                                           varRef->get_type());
   SgUnaryOp *uOp = isSgUnaryOp(parent);
   if (uOp != NULL) {
      uOp->set_operand(commaOp);
   }
   else {
      SgBinaryOp *bOp = isSgBinaryOp(parent);
      if (bOp != NULL) {
         if (bOp->get_lhs_operand() == varRef) {
            bOp->set_lhs_operand(commaOp);
         }
         else {
            assert(bOp->get_rhs_operand() == varRef);
            bOp->set_rhs_operand(commaOp);
         }
      }
      else {
         SgExprListExp *expList = isSgExprListExp(parent);
         if (expList != NULL) {
            SgExpressionPtrList& expressions = expList->get_expressions();
            for (SgExpressionPtrList::iterator iter = expressions.begin(); ; iter++) {
               assert (iter != expressions.end()); //element must be in the list!
               if (*iter == varRef) {
                  //insert commaOp instead of varRef
                  expressions.insert(expressions.erase(iter), commaOp);
                  break;
               }
            }
         }
         else {
            //SgClassNameRefExp
            //SgConditionalExp
            //SgDeleteExp
            //go on implementing other cases
            cerr<<"unexpected parent expression: "<<parent->class_name()<<endl;
            assert (false);
         }
      }
   }
}


class SynthesizedAttribute {
public:
   bool isVarRef;
   SgNode *node;
   SynthesizedAttribute(): isVarRef(false), node(NULL) {};
};

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
       // Functions required
          InheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             InheritedAttribute inheritedAttribute );

          SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             InheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

InheritedAttribute
Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
     return inheritedAttribute;
   }

SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
         SgNode* astNode,
         InheritedAttribute inheritedAttribute,
         SubTreeSynthesizedAttributes synthesizedAttributeList ) {
   SynthesizedAttribute returnAttribute;
   returnAttribute.node = astNode;
   SgVarRefExp *varRef = NULL; //set to a var ref child of astNode iff this child is to be
                              //instrumented
   if (isSgVarRefExp(astNode)) {
      returnAttribute.isVarRef = true;
   }
   else {
      if (isSgAssignOp(astNode)) { //add other assignment operatrors
         SynthesizedAttribute leftChild = synthesizedAttributeList[SgBinaryOp_lhs_operand_i];
         if (leftChild.isVarRef) {
            varRef = isSgVarRefExp(leftChild.node);
            assert(varRef != NULL);
            instrumentWrite(varRef);
         }
      }
      if (varRef == NULL) { //not else! could be right-hand-side of assignment
         for (SubTreeSynthesizedAttributes::iterator iter = synthesizedAttributeList.begin();
                                                iter != synthesizedAttributeList.end(); iter++) {
            if (iter->isVarRef) {
               varRef = isSgVarRefExp(iter->node);
               assert (varRef != NULL);
               instrumentRead(varRef);
            }
         }
      }
   }
   return returnAttribute;
}


int main ( int argc, char* argv[] ) {
   // Initialize and check compatibility. See rose::initialize
   ROSE_INITIALIZE;

   SgProject* project = frontend(argc,argv);
   ROSE_ASSERT (project != NULL);

   // Call function to declare function to be called to recode use of all functions in the AST
   beforeWrite.buildDeclaration(project);
   beforeRead.buildDeclaration(project);

   // Build the inherited attribute
   InheritedAttribute inheritedAttribute;

   Traversal myTraversal;

   // Call the traversal starting at the sageProject node of the AST
   myTraversal.traverseInputFiles(project,inheritedAttribute);

   // Generate Code and compile it with backend (vendor) compiler to generate object code
   // or executable (as specified on commandline using vendor compiler's command line).
   // Returns error code form compilation using vendor's compiler.
   return backend(project);
}
