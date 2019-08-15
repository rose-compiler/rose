#include "rose.h"

#include "astStructRecognition.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilderAsm;

// Constructor
DataMemberInitializationAttribute::DataMemberInitializationAttribute ( SgAsmInstruction* instruction, SgProject* p )
   {
     ROSE_ASSERT(instruction != NULL);
     printf ("Building a DataMemberInitializationAttribute object for instruction = %s \n",unparseInstructionWithAddress(instruction).c_str());

  // Identify the offset and if this is a stack reference or a global reference.
     SgAsmMemoryReferenceExpression* memoryReferenceExpression = isSgAsmMemoryReferenceExpression(instruction->operand(0));
     ROSE_ASSERT(memoryReferenceExpression != NULL);

     SgAsmRegisterReferenceExpression* segmentRegister = isSgAsmRegisterReferenceExpression(memoryReferenceExpression->get_segment());
     ROSE_ASSERT(segmentRegister != NULL);

#ifdef USE_NEW_ISA_INDEPENDENT_REGISTER_HANDLING
  // This is a data member
     isStackVariable  = (segmentRegister->get_register_number() == SgAsmRegisterReferenceExpression::e_ss);

  // This is to test if there is another register being referenced.
     bool isGlobalVariable = (segmentRegister->get_register_number() == SgAsmRegisterReferenceExpression::e_ds);

  // This should be either a stack or global variable (no other allowed).
     ROSE_ASSERT(isStackVariable == true  || isGlobalVariable == true);
     ROSE_ASSERT(isStackVariable == false || isGlobalVariable == false);

     SgAsmBinaryAdd* binaryAdd = isSgAsmBinaryAdd(memoryReferenceExpression->get_address());
     if (binaryAdd != NULL)
        {
       // Case of a non-zero offset into the global scope or the stack frame. 
          ROSE_ASSERT(binaryAdd->get_rhs() != NULL);
          SgAsmValueExpression* offsetExpression = isSgAsmValueExpression(binaryAdd->get_rhs());
          ROSE_ASSERT(offsetExpression != NULL);
          offset = get_value(offsetExpression);
        }
       else
        {
       // Case of zero offset from stack frame (first variable in the stack) 
       // or a global variable with immediate address.
          offset = 0;
          if (isGlobalVariable == true)
             {
             // Handle the case of a global variable.
                ROSE_ASSERT(memoryReferenceExpression != NULL);

                SgAsmValueExpression* offsetExpression = isSgAsmValueExpression(memoryReferenceExpression->get_address());
                if (offsetExpression != NULL)
                   {
                     offset = get_value(offsetExpression);
                   }

#if 0
             // ... finish me ...
                printf ("... finish me ... offset = %p \n",(void*)offset);
                ROSE_ASSERT(false);
#else
                printf ("isGlobalVariable == true: offset = %p \n",(void*)offset);
#endif
             }
        }

     SgAsmValueExpression* valueExpression = isSgAsmValueExpression(instruction->operand(1));
     ROSE_ASSERT(valueExpression != NULL);

     value = get_value(valueExpression);

     type = valueExpression->get_type();
     ROSE_ASSERT(type != NULL);

     printf ("   isStackVariable  = %s \n",isStackVariable ? "true" : "false");
     printf ("   isGlobalVariable = %s \n",isGlobalVariable ? "true" : "false");
     printf ("   offset           = %zu \n",offset);
     printf ("   value            = %zu = %p \n",value,(void*)value);
     printf ("   type             = %p = %s \n",type,type->class_name().c_str());
#else
  // DQ (9/2/2013): This allows us to get the existing code compiled and then move to update the code seperately.
     printf ("This code needs to be updated to handle the new register handling (ISA independence) \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("Exiting as a test... \n");
     ROSE_ASSERT(false);
#endif
   }


TypeAnalysisAttribute::TypeAnalysisAttribute(size_t addr, SgAsmType* type)
   {
  // This attribute is used to record types on the stack and global scope.
     address = addr;
     asmType = type;

     ROSE_ASSERT(asmType != NULL);
   }


// ****************************************************************
// Support functions for building patterns checked against the AST.
// ****************************************************************
vector<SgNode*>
dataMemberInitializationWithoutOffsetSupport( SgProject* project, SgAsmRegisterReferenceExpression::regnames32_enum registerName,
                                              SgAsmRegisterReferenceExpression::segregnames_enum segmentRegister )
   {
  // This is a data member initialization in a member function of a data structure on the stack

  // TODO: Next I need to permit the type to be specified through the function interface.
  //       Returning a vector<SgAsmInstruction*> might make for a better function interface.

     using namespace SageBuilderAsm;

#ifdef USE_NEW_ISA_INDEPENDENT_REGISTER_HANDLING
     SgAsmInstruction* target = buildX86Instruction(x86_mov,
                   buildAsmMemoryReferenceExpression(
                        buildAsmRegisterReferenceExpression(x86_regclass_gpr,registerName),
                        buildAsmRegisterReferenceExpression(x86_regclass_segment,segmentRegister),
                     // This value should match the type below.
                        buildTypeX86DoubleWord() ),
                // This value will be ignored in matching (but the type will be checked).
                // To provide more flexability we could use a function pointer as a function 
                // parameter, but it would be difficult ot match types.  Better ot just specify 
                // a type via an enum value (and then internally build the matching value and type).
                   buildValueX86DWord(0x8048858));

     printf ("In dataMemberInitializationWithoutOffset() target instruction = %s \n",unparseInstructionWithAddress(target).c_str());

  // General function to find matching target AST in larger AST.
     vector<SgNode*> l = find ( project, target, equivalenceTest );

     printf ("Leaving dataMemberInitializationWithoutOffset() \n");
     return l;
#else
  // DQ (9/2/2013): This allows us to get the existing code compiled and then move to update the code seperately.
     printf ("This code needs to be updated to handle the new register handling (ISA independence) \n");
     ROSE_ASSERT(false);

     return vector<SgNode*>();
#endif
   }

vector<SgNode*>
dataMemberInitializationWithOffsetSupport( SgProject* project, SgAsmRegisterReferenceExpression::regnames32_enum registerName,
                                           SgAsmRegisterReferenceExpression::segregnames_enum segmentRegister )
   {
  // This is a data member initialization in a member function of a data structure on the stack

  // TODO: Next I need to permit the type to be specified through the function interface.
  //       Returning a vector<SgAsmInstruction*> might make for a better function interface.

     using namespace SageBuilderAsm;

#ifdef USE_NEW_ISA_INDEPENDENT_REGISTER_HANDLING
     SgAsmInstruction* target = buildX86Instruction(x86_mov,
                   buildAsmMemoryReferenceExpression(
                        buildAsmAddExpression(
                          // buildAsmRegisterReferenceExpression(x86_regclass_gpr,SgAsmRegisterReferenceExpression::e_eax),
                             buildAsmRegisterReferenceExpression(x86_regclass_gpr,registerName),
                          // This value will be ignored in matching.
                             buildValueX86Byte(0x04)),
                     // buildAsmRegisterReferenceExpression(x86_regclass_segment,SgAsmRegisterReferenceExpression::e_ds),
                        buildAsmRegisterReferenceExpression(x86_regclass_segment,segmentRegister),
                     // Note that the type will not be checked since it is not traversed in the AST.
                        buildTypeX86DoubleWord() ),
                // This value will be ignored in matching (but the type will be checked).
                   buildValueX86DWord(0x8048858));

     printf ("In dataMemberInitializationWithOffset(): target instruction = %s \n",unparseInstructionWithAddress(target).c_str());

  // General function to find matching target AST in larger AST.
     vector<SgNode*> l = find ( project, target, equivalenceTest );

     printf ("Leaving dataMemberInitializationWithOffset() \n");
     return l;
#else
  // DQ (9/2/2013): This allows us to get the existing code compiled and then move to update the code seperately.
     printf ("This code needs to be updated to handle the new register handling (ISA independence) \n");
     ROSE_ASSERT(false);

     return vector<SgNode*>();
#endif
   }

vector<SgNode*>
dataMemberInitializationUsingAddressSupport( SgProject* project, SgAsmRegisterReferenceExpression::segregnames_enum segmentRegister )
   {
  // This is a data member initialization in either a member function or regular function of a data structure in the global scope.

  // TODO: Next I need to permit the type to be specified through the function interface.
  //       Returning a vector<SgAsmInstruction*> might make for a better function interface.

     using namespace SageBuilderAsm;

#ifdef USE_NEW_ISA_INDEPENDENT_REGISTER_HANDLING
     SgAsmInstruction* target = buildX86Instruction(x86_mov,
                   buildAsmMemoryReferenceExpression(
                     // This value will be ignored in matching.
                        buildValueX86DWord(0x8049c8c),
                        buildAsmRegisterReferenceExpression(x86_regclass_segment,segmentRegister),
                        buildTypeX86DoubleWord() ),
                // This value will be ignored in matching (but the type will be checked).
                   buildValueX86DWord(0x8048858)->addRegExpAttribute("regex",new AstRegExAttribute("*")));

     printf ("In dataMemberInitializationWithoutOffset() target instruction = %s \n",unparseInstructionWithAddress(target).c_str());

  // General function to find matching target AST in larger AST.
     vector<SgNode*> l = find ( project, target, equivalenceTest );

     printf ("Leaving dataMemberInitializationWithoutOffset() \n");
     return l;
#else
  // DQ (9/2/2013): This allows us to get the existing code compiled and then move to update the code seperately.
     printf ("This code needs to be updated to handle the new register handling (ISA independence) \n");
     ROSE_ASSERT(false);

     return vector<SgNode*>();
#endif
   }

void
setupInstructionAttributes ( SgProject* project, vector<SgNode*> instructionList, string comment )
   {
  // Refactored code to setup DataMemberInitializationAttribute objects.

     ROSE_ASSERT(project != NULL);

     printf ("dataMemberInitializationWithoutOffsetInMemberFunction(): instructionList.size() = %zu \n",instructionList.size());

  // Keep track of previously referenced variables to avoid redeclaring them in code generation.
  // set<size_t> usedGlobalOffsets;

     for (size_t i = 0; i < instructionList.size(); i++)
        {
       // Get the SgAsmInstruction
          SgAsmInstruction* instruction = isSgAsmInstruction(instructionList[i]);
          ROSE_ASSERT(instruction != NULL);

       // ROSE_ASSERT(instruction != target);
       // printf ("Processing instruction %s \n",unparseInstructionWithAddress(target).c_str());

       // Build an attribute (on the heap)
       // AstAttribute* newAttribute = new VirtualTableLoad(instruction);
          DataMemberInitializationAttribute* variableAttribute = new DataMemberInitializationAttribute(instruction,project);
          ROSE_ASSERT(variableAttribute != NULL);

       // Add it to the AST (so it can be found later in another pass over the AST)
          instruction->addNewAttribute("DataMemberInitializationAttribute",variableAttribute);

          ROSE_ASSERT(variableAttribute->type != NULL);

       // string comment = "Data member initialization in member function: ";
       // printf ("Attaching comment = %s \n",comment.c_str());
          addComment(instruction,comment);
        }
   }

void
setupVariableDeclartionAttributes ( SgProject* project, vector<SgNode*> instructionList )
   {
  // Refactored code to setup DataMemberInitializationAttribute objects.

     ROSE_ASSERT(project != NULL);

     printf ("dataMemberInitializationWithoutOffsetInMemberFunction(): instructionList.size() = %zu \n",instructionList.size());

  // Keep track of previously referenced variables to avoid redeclaring them in code generation.
  // set<size_t> usedGlobalOffsets;

     for (size_t i = 0; i < instructionList.size(); i++)
        {
       // Add another comment.
          string comment = "This is a reference to variable: ";

       // Get the SgAsmInstruction
          SgAsmInstruction* instruction = isSgAsmInstruction(instructionList[i]);
          ROSE_ASSERT(instruction != NULL);

          DataMemberInitializationAttribute* variableAttribute = dynamic_cast<DataMemberInitializationAttribute*>(instruction->getAttribute("DataMemberInitializationAttribute"));
          ROSE_ASSERT(variableAttribute != NULL);

       // Now add the analysis attribute that represents variables referenced in the binary.
          if (variableAttribute->isStackVariable == true)
             {
            // This is a stack variable reference defined in a function.

            // DQ (9/2/2013): Modified to reflect name change in SgAsmFunctionDeclaration to SgAsmFunction.
            // SgAsmFunctionDeclaration* functionDeclaration = getAsmFunctionDeclaration(instruction);
               SgAsmFunction* functionDeclaration = getAsmFunction(instruction);
               ROSE_ASSERT(functionDeclaration != NULL);

            // This can be either a FunctionAnalysisAttribute or MemberFunctionAnalysisAttribute pointer.
               FunctionAnalysisAttribute* functionAnalysisAttribute = dynamic_cast<FunctionAnalysisAttribute*>(functionDeclaration->getAttribute("FunctionAnalysisAttribute"));
               ROSE_ASSERT(functionAnalysisAttribute != NULL);

            // Add it to the AST (so it can be found later in another pass over the AST)
            // functionDeclaration->addNewAttribute("FunctionAnalysisAttribute",functionAnalysisAttribute);

            // Build an analysis attribute specific to the type of each stack variable reference.
               TypeAnalysisAttribute* asmTypeAttribute = new TypeAnalysisAttribute(variableAttribute->offset,variableAttribute->type);
               ROSE_ASSERT(asmTypeAttribute != NULL);

            // Make up a name for the variable.
            // asmTypeAttribute->name = "stackVar_" + StringUtility::numberToString(i);
               asmTypeAttribute->name = "stackVar_" + generateUniqueName ( variableAttribute->offset, functionAnalysisAttribute->usedScopeOffsets, functionAnalysisAttribute->variableCounter );

            // Improve the comments by adding the stack variable name.
               comment += " variable: " + asmTypeAttribute->name;

            // Add this variable's type to the list of variable types with addresses on the stack.
               functionAnalysisAttribute->stackTypeList.push_back(asmTypeAttribute);
             }
            else
             {
            // This is a global variable reference (used from anywhere in the program).

            // Verify we have a valid pointer (global variable).
               ROSE_ASSERT(globalScopeAttribute != NULL);

               size_t offset = variableAttribute->offset;

            // if (usedGlobalOffsets.find(offset) == usedGlobalOffsets.end())
                  {
                 // Add this entry so that it will not be reused.
                 // usedGlobalOffsets.insert(offset);

                    TypeAnalysisAttribute* asmTypeAttribute = new TypeAnalysisAttribute(offset,variableAttribute->type);
                    ROSE_ASSERT(asmTypeAttribute != NULL);

                 // Make up a name for the variable.
                 // asmTypeAttribute->name = "globalVar_" + StringUtility::numberToString(i);
                    asmTypeAttribute->name = "globalVar_" + generateUniqueName ( variableAttribute->offset, globalScopeAttribute->usedScopeOffsets, globalScopeAttribute->variableCounter );

                 // Improve the comments by adding the globald variable name.
                    comment += " variable: " + asmTypeAttribute->name;

                 // Add this variable's type to the list of variable types with addresses on the stack.
                    globalScopeAttribute->stackTypeList.push_back(asmTypeAttribute);
                  }
             }

       // string comment = "Data member initialization in member function: ";
       // printf ("Attaching comment = %s \n",comment.c_str());
          addComment(instruction,comment);
        }
   }

// *******************************************************************************************
// Higher level API for the generation of lists of instructions that match a specific pattern.
// *******************************************************************************************
void
dataMemberInitializationWithoutOffsetInMemberFunction( SgProject* project)
   {
  // This is a data member initialization in a member function of its data structure.
     vector<SgNode*> instructionList = dataMemberInitializationWithoutOffsetSupport(project,SgAsmRegisterReferenceExpression::e_eax,SgAsmRegisterReferenceExpression::e_ds);

     setupInstructionAttributes(project,instructionList,"Data segment (global variable) initialization (without offset) in member function");

     setupVariableDeclartionAttributes (project,instructionList);
   }

void
dataMemberInitializationWithOffsetInMemberFunction( SgProject* project )
   {
  // This is a data member initialization in a member function of its data structure.
     vector<SgNode*> instructionList = dataMemberInitializationWithOffsetSupport(project,SgAsmRegisterReferenceExpression::e_eax,SgAsmRegisterReferenceExpression::e_ds);

     setupInstructionAttributes(project,instructionList,"Data segment (global variable) initialization (with offset) in member function");

     setupVariableDeclartionAttributes (project,instructionList);
   }

void
dataMemberInitializationWithoutOffsetInFunction( SgProject* project )
   {
  // This is a data member initialization in a normal function of a data structure on the stack
     vector<SgNode*> instructionList = dataMemberInitializationWithoutOffsetSupport(project,SgAsmRegisterReferenceExpression::e_ebp,SgAsmRegisterReferenceExpression::e_ss);

  // DQ (11/8/2009): It seems that this is always true since the compiler will not generate this type of instruction without an offset from "ebp".
     ROSE_ASSERT(instructionList.empty() == true);

     setupInstructionAttributes(project,instructionList,"Data member initialization in function (stack reference)");

     setupVariableDeclartionAttributes (project,instructionList);
   }

void
dataMemberInitializationWithOffsetInFunction( SgProject* project )
   {
  // This is a data member initialization in a normal function of a data structure on the stack
     vector<SgNode*> instructionList = dataMemberInitializationWithOffsetSupport(project,SgAsmRegisterReferenceExpression::e_ebp,SgAsmRegisterReferenceExpression::e_ss);

     setupInstructionAttributes(project,instructionList,"Data member initialization in function (stack reference)");

     setupVariableDeclartionAttributes (project,instructionList);
   }

void
dataMemberInitializationUsingAddress( SgProject* project )
   {
  // This is a data member initialization in any function of a data structure in global scope.
     vector<SgNode*> instructionList = dataMemberInitializationUsingAddressSupport(project,SgAsmRegisterReferenceExpression::e_ds);

     setupInstructionAttributes(project,instructionList,"Data member initialization in any function (global variable reference)");

     setupVariableDeclartionAttributes (project,instructionList);
   }

void
stackDataInitializationUsingAddress( SgProject* project )
   {
  // This is a simpler primative type data initialization for data on the stack in any function.
  // Since this is not about data structure initialization we might separate it out (refactor) later.
  // Looking for mov    WORD PTR ss:[ebp + *], *
     vector<SgNode*> instructionList = dataMemberInitializationUsingAddressSupport(project,SgAsmRegisterReferenceExpression::e_ss);

     setupInstructionAttributes(project,instructionList,"Data member initialization in any function (stack variable reference)");

     setupVariableDeclartionAttributes (project,instructionList);
   }

// Main function to detect different types of instructions used to initialize data
void
detectVariableInitialization( SgProject* project )
   {
     printf ("Inside of detectStructureFieldInitialization() \n");

  // Detection in member functions of access to the data members in the same class.
     dataMemberInitializationWithoutOffsetInMemberFunction(project);
     dataMemberInitializationWithOffsetInMemberFunction(project);

  // Detection in regular functions (data structures on the stack).
     dataMemberInitializationWithOffsetInFunction(project);
  // Note that "dataMemberInitializationWithoutOffsetInFunction()" will internally compute an empty list, but we verify this.
     dataMemberInitializationWithoutOffsetInFunction(project);

  // Detection in either regular functions or  member functions of initialization of data members in global scope.
     dataMemberInitializationUsingAddress(project);

  // Detection of initialization of data on the stack (not about data structures).
  // This needs to support different types so that we can bak out type information.
  // There are 10 possbiel types supported in ROSE for the binary ASM work.
     stackDataInitializationUsingAddress(project);
   }

