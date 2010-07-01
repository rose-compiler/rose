#include "rose.h"

#define MAINFILE
static SgBasicBlock* bb; // Global location to append new statements

static size_t WordWithExpression_nameCounter = 0;


#include "x86AssemblyToC.h"
#include "integerOps.h"
#include <iostream>

using namespace std;
using namespace IntegerOps;
using namespace SageInterface;
using namespace SageBuilder;
#if 1
set<SgInitializedName*> makeAllPossibleVars(const X86CTranslationPolicy& conv) {
  set<SgInitializedName*> result;
  for (size_t i = 0; i < 16; ++i) {
    if (conv.gprSym[i]) {
      result.insert(conv.gprSym[i]->get_declaration());
    }
  }
  for (size_t i = 0; i < conv.nflags; ++i) {
    if (conv.flagsSym[i]) {
      result.insert(conv.flagsSym[i]->get_declaration());
    }
  }
   result.insert(conv.ipSym->get_declaration());
  result.insert(conv.sf_xor_ofSym->get_declaration());
  if(conv.zf_or_cfSym != NULL)
  result.insert(conv.zf_or_cfSym->get_declaration());
  return result;
}
#endif

#if 1
set<SgInitializedName*> computeLiveVars(SgStatement* stmt, const X86CTranslationPolicy& conv, map<SgLabelStatement*, set<SgInitializedName*> >& liveVarsForLabels, set<SgInitializedName*> currentLiveVars, bool actuallyRemove) {
  switch (stmt->variantT()) {
    case V_SgBasicBlock: {
      const SgStatementPtrList& stmts = isSgBasicBlock(stmt)->get_statements();
      for (size_t i = stmts.size(); i > 0; --i) {
        currentLiveVars = computeLiveVars(stmts[i - 1], conv, liveVarsForLabels, currentLiveVars, actuallyRemove);
      }
      return currentLiveVars;
    }
    case V_SgPragmaDeclaration: return currentLiveVars;
    case V_SgDefaultOptionStmt: return currentLiveVars;
    case V_SgCaseOptionStmt: {
      return computeLiveVars(isSgCaseOptionStmt(stmt)->get_body(), conv, liveVarsForLabels, currentLiveVars, actuallyRemove);
    }
    case V_SgLabelStatement: {
      liveVarsForLabels[isSgLabelStatement(stmt)] = currentLiveVars;
      return currentLiveVars;
    }
    case V_SgGotoStatement: {
      return liveVarsForLabels[isSgGotoStatement(stmt)->get_label()];
    }
    case V_SgSwitchStatement: {
      SgSwitchStatement* s = isSgSwitchStatement(stmt);
      SgBasicBlock* swBody = isSgBasicBlock(s->get_body());
      ROSE_ASSERT (swBody);
      const SgStatementPtrList& bodyStmts = swBody->get_statements();
      set<SgInitializedName*> liveForBody; // Assumes any statement in the body is possible
      for (size_t i = 0; i < bodyStmts.size(); ++i) {
        setUnionInplace(liveForBody, computeLiveVars(bodyStmts[i], conv, liveVarsForLabels, currentLiveVars, actuallyRemove));
      }
      return computeLiveVars(s->get_item_selector(), conv, liveVarsForLabels, liveForBody, actuallyRemove);
    }
    case V_SgContinueStmt: {
      return makeAllPossibleVars(conv);
    }
    case V_SgIfStmt: {
      set<SgInitializedName*> liveForBranches = computeLiveVars(isSgIfStmt(stmt)->get_true_body(), conv, liveVarsForLabels, currentLiveVars, actuallyRemove);
      setUnionInplace(liveForBranches, (isSgIfStmt(stmt)->get_false_body() != NULL ? computeLiveVars(isSgIfStmt(stmt)->get_false_body(), conv, liveVarsForLabels, currentLiveVars, actuallyRemove) : set<SgInitializedName*>()));
      return computeLiveVars(isSgIfStmt(stmt)->get_conditional(), conv, liveVarsForLabels, liveForBranches, actuallyRemove);
    }
    case V_SgWhileStmt: {
      while (true) {
        set<SgInitializedName*> liveVarsSave = currentLiveVars;
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_body(), conv, liveVarsForLabels, currentLiveVars, false);
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_condition(), conv, liveVarsForLabels, currentLiveVars, false);
        setUnionInplace(currentLiveVars, liveVarsSave);
        if (liveVarsSave == currentLiveVars) break;
      }
      if (actuallyRemove) {
        set<SgInitializedName*> liveVarsSave = currentLiveVars;
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_body(), conv, liveVarsForLabels, currentLiveVars, true);
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_condition(), conv, liveVarsForLabels, currentLiveVars, true);
        setUnionInplace(currentLiveVars, liveVarsSave);
      }
      return currentLiveVars;
    }
    case V_SgBreakStmt: return set<SgInitializedName*>();
    case V_SgExprStatement: {
      SgExpression* e = isSgExprStatement(stmt)->get_expression();
      switch (e->variantT()) {
        case V_SgAssignOp: {
          SgVarRefExp* lhs = isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand());
          ROSE_ASSERT (lhs);
          SgInitializedName* in = lhs->get_symbol()->get_declaration();
          if (currentLiveVars.find(in) == currentLiveVars.end()) {
            if (actuallyRemove) {
              // cerr << "Removing assignment " << e->unparseToString() << endl;
              isSgStatement(stmt->get_parent())->remove_statement(stmt);
            }
            return currentLiveVars;
          } else {
            currentLiveVars.erase(in);
            getUsedVariables(isSgAssignOp(e)->get_rhs_operand(), currentLiveVars);
            return currentLiveVars;
          }
        }
        case V_SgFunctionCallExp: {
          getUsedVariables(e, currentLiveVars);
          SgFunctionRefExp* fr = isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function());
          ROSE_ASSERT (fr);
          if (fr->get_symbol()->get_declaration() == conv.interruptSym->get_declaration()) {
            setUnionInplace(currentLiveVars, makeAllPossibleVars(conv));
            return currentLiveVars;
          } else {
            return currentLiveVars;
          }
        }
        default: {
          getUsedVariables(e, currentLiveVars);
          return currentLiveVars;
        }
      }
    }
    case V_SgVariableDeclaration: {
      ROSE_ASSERT (isSgVariableDeclaration(stmt)->get_variables().size() == 1);
      SgInitializedName* in = isSgVariableDeclaration(stmt)->get_variables()[0];
      bool isConst = isConstType(in->get_type());
      if (currentLiveVars.find(in) == currentLiveVars.end() && isConst) {
        if (actuallyRemove) {
          // cerr << "Removing decl " << stmt->unparseToString() << endl;
          isSgStatement(stmt->get_parent())->remove_statement(stmt);
        }
        return currentLiveVars;
      } else {
        currentLiveVars.erase(in);
        if (in->get_initializer()) {
          getUsedVariables(in->get_initializer(), currentLiveVars);
        }
        return currentLiveVars;
      }
    }
    default: cerr << "computeLiveVars: " << stmt->class_name() << endl; abort();
  }
}

void removeDeadStores(SgBasicBlock* switchBody, const X86CTranslationPolicy& conv) {
  map<SgLabelStatement*, set<SgInitializedName*> > liveVars;
  while (true) {
    map<SgLabelStatement*, set<SgInitializedName*> > liveVarsSave = liveVars;
    computeLiveVars(switchBody, conv, liveVars, set<SgInitializedName*>(), false);
    if (liveVars == liveVarsSave) break;
  }
  computeLiveVars(switchBody, conv, liveVars, set<SgInitializedName*>(), true);
}
#endif




// This builds variable declarations and variable references for each symbolic value
// These are returnd and taken as arguments by the primative functions.
// Operations build IR nodes to represent the expressions.

#if 0 // Unused
static int sizeOfInsnSize(X86InstructionSize s) {
  switch (s) {
    case x86_insnsize_16: return 2;
    case x86_insnsize_32: return 4;
    case x86_insnsize_64: return 8;
    default: ROSE_ASSERT (!"sizeOfInsnSize");
  }
}
#endif

SgFunctionSymbol* X86CTranslationPolicy::addHelperFunction(const std::string& name, SgType* returnType, SgFunctionParameterList* params) {
  SgFunctionDeclaration* decl = buildNondefiningFunctionDeclaration(name, returnType, params, globalScope);
  appendStatement(decl, globalScope);
  SgFunctionSymbol* sym = globalScope->lookup_function_symbol(name);
  ROSE_ASSERT (sym);
  return sym;
}

X86CTranslationPolicy::X86CTranslationPolicy(SgSourceFile* f, SgAsmGenericFile* asmFile): asmFile(asmFile), globalScope(NULL) {
  ROSE_ASSERT (f);
  ROSE_ASSERT (f->get_globalScope());
  globalScope = f->get_globalScope();
#define LOOKUP_FUNC(name) \
  do {name##Sym = globalScope->lookup_function_symbol(#name); ROSE_ASSERT (name##Sym);} while (0)
  LOOKUP_FUNC(parity);
  LOOKUP_FUNC(mulhi16);
  LOOKUP_FUNC(mulhi32);
  LOOKUP_FUNC(mulhi64);
  LOOKUP_FUNC(imulhi16);
  LOOKUP_FUNC(imulhi32);
  LOOKUP_FUNC(imulhi64);
  LOOKUP_FUNC(div8);
  LOOKUP_FUNC(mod8);
  LOOKUP_FUNC(div16);
  LOOKUP_FUNC(mod16);
  LOOKUP_FUNC(div32);
  LOOKUP_FUNC(mod32);
  LOOKUP_FUNC(div64);
  LOOKUP_FUNC(mod64);
  LOOKUP_FUNC(idiv8);
  LOOKUP_FUNC(imod8);
  LOOKUP_FUNC(idiv16);
  LOOKUP_FUNC(imod16);
  LOOKUP_FUNC(idiv32);
  LOOKUP_FUNC(imod32);
  LOOKUP_FUNC(idiv64);
  LOOKUP_FUNC(imod64);
  LOOKUP_FUNC(bsr);
  LOOKUP_FUNC(bsf);
  gprSym[0] = globalScope->lookup_variable_symbol("rax"); ROSE_ASSERT (gprSym[0]);
  gprSym[1] = globalScope->lookup_variable_symbol("rcx"); ROSE_ASSERT (gprSym[1]);
  gprSym[2] = globalScope->lookup_variable_symbol("rdx"); ROSE_ASSERT (gprSym[2]);
  gprSym[3] = globalScope->lookup_variable_symbol("rbx"); ROSE_ASSERT (gprSym[3]);
  gprSym[4] = globalScope->lookup_variable_symbol("rsp"); ROSE_ASSERT (gprSym[4]);
  gprSym[5] = globalScope->lookup_variable_symbol("rbp"); ROSE_ASSERT (gprSym[5]);
  gprSym[6] = globalScope->lookup_variable_symbol("rsi"); ROSE_ASSERT (gprSym[6]);
  gprSym[7] = globalScope->lookup_variable_symbol("rdi"); ROSE_ASSERT (gprSym[7]);
  gprSym[8] = globalScope->lookup_variable_symbol("r8"); ROSE_ASSERT (gprSym[8]);
  gprSym[9] = globalScope->lookup_variable_symbol("r9"); ROSE_ASSERT (gprSym[9]);
  gprSym[10] = globalScope->lookup_variable_symbol("r10"); ROSE_ASSERT (gprSym[10]);
  gprSym[11] = globalScope->lookup_variable_symbol("r11"); ROSE_ASSERT (gprSym[11]);
  gprSym[12] = globalScope->lookup_variable_symbol("r12"); ROSE_ASSERT (gprSym[12]);
  gprSym[13] = globalScope->lookup_variable_symbol("r13"); ROSE_ASSERT (gprSym[13]);
  gprSym[14] = globalScope->lookup_variable_symbol("r14"); ROSE_ASSERT (gprSym[14]);
  gprSym[15] = globalScope->lookup_variable_symbol("r15"); ROSE_ASSERT (gprSym[15]);

  /* Complete set of Intel Pentium flag bits in the "eflags" register */
  ROSE_ASSERT(nflags==32); /* be sure to define all of them below */
  flagsSym[0] = globalScope->lookup_variable_symbol("cf"); ROSE_ASSERT (flagsSym[0]);
  flagsSym[1] = globalScope->lookup_variable_symbol("f1"); ROSE_ASSERT(flagsSym[1]);    /*usually called "1f"*/
  flagsSym[2] = globalScope->lookup_variable_symbol("pf"); ROSE_ASSERT (flagsSym[2]);
  flagsSym[3] = globalScope->lookup_variable_symbol("f3"); ROSE_ASSERT(flagsSym[3]);    /*usually called "3f"*/
  flagsSym[4] = globalScope->lookup_variable_symbol("af"); ROSE_ASSERT (flagsSym[4]);
  flagsSym[5] = globalScope->lookup_variable_symbol("f5"); ROSE_ASSERT(flagsSym[5]);    /*usually called "5f"*/
  flagsSym[6] = globalScope->lookup_variable_symbol("zf"); ROSE_ASSERT (flagsSym[6]);
  flagsSym[7] = globalScope->lookup_variable_symbol("sf"); ROSE_ASSERT (flagsSym[7]);
  flagsSym[8] = globalScope->lookup_variable_symbol("tf"); ROSE_ASSERT(flagsSym[8]);
  flagsSym[9] = globalScope->lookup_variable_symbol("iflg"); ROSE_ASSERT(flagsSym[9]);  /*usually called "if"*/
  flagsSym[10] = globalScope->lookup_variable_symbol("df"); ROSE_ASSERT (flagsSym[10]);
  flagsSym[11] = globalScope->lookup_variable_symbol("of"); ROSE_ASSERT (flagsSym[11]);
  flagsSym[12] = globalScope->lookup_variable_symbol("iopl0"); ROSE_ASSERT(flagsSym[12]); /*this and iopl1 are a two-bit field*/
  flagsSym[13] = globalScope->lookup_variable_symbol("iopl1"); ROSE_ASSERT(flagsSym[13]);
  flagsSym[14] = globalScope->lookup_variable_symbol("nt"); ROSE_ASSERT(flagsSym[14]);
  flagsSym[15] = globalScope->lookup_variable_symbol("f15"); ROSE_ASSERT(flagsSym[15]); /*usually called "15f"*/
  flagsSym[16] = globalScope->lookup_variable_symbol("rf"); ROSE_ASSERT(flagsSym[16]);
  flagsSym[17] = globalScope->lookup_variable_symbol("vm"); ROSE_ASSERT(flagsSym[17]);
  flagsSym[18] = globalScope->lookup_variable_symbol("ac"); ROSE_ASSERT(flagsSym[18]);
  flagsSym[19] = globalScope->lookup_variable_symbol("vif"); ROSE_ASSERT(flagsSym[19]);
  flagsSym[20] = globalScope->lookup_variable_symbol("vip"); ROSE_ASSERT(flagsSym[20]);
  flagsSym[21] = globalScope->lookup_variable_symbol("id"); ROSE_ASSERT(flagsSym[21]);
  flagsSym[22] = globalScope->lookup_variable_symbol("f22"); ROSE_ASSERT(flagsSym[22]); /*usually called "22f", etc...*/
  flagsSym[23] = globalScope->lookup_variable_symbol("f23"); ROSE_ASSERT(flagsSym[23]);
  flagsSym[24] = globalScope->lookup_variable_symbol("f24"); ROSE_ASSERT(flagsSym[24]);
  flagsSym[25] = globalScope->lookup_variable_symbol("f25"); ROSE_ASSERT(flagsSym[25]);
  flagsSym[26] = globalScope->lookup_variable_symbol("f26"); ROSE_ASSERT(flagsSym[26]);
  flagsSym[27] = globalScope->lookup_variable_symbol("f27"); ROSE_ASSERT(flagsSym[27]);
  flagsSym[28] = globalScope->lookup_variable_symbol("f28"); ROSE_ASSERT(flagsSym[28]);
  flagsSym[29] = globalScope->lookup_variable_symbol("f29"); ROSE_ASSERT(flagsSym[29]);
  flagsSym[30] = globalScope->lookup_variable_symbol("f30"); ROSE_ASSERT(flagsSym[30]);
  flagsSym[31] = globalScope->lookup_variable_symbol("f31"); ROSE_ASSERT(flagsSym[31]);

  ipSym = globalScope->lookup_variable_symbol("ip"); ROSE_ASSERT (ipSym);
  LOOKUP_FUNC(memoryReadByte);
  LOOKUP_FUNC(memoryReadWord);
  LOOKUP_FUNC(memoryReadDWord);
  LOOKUP_FUNC(memoryReadQWord);
  LOOKUP_FUNC(memoryWriteByte);
  LOOKUP_FUNC(memoryWriteWord);
  LOOKUP_FUNC(memoryWriteDWord);
  LOOKUP_FUNC(memoryWriteQWord);
  LOOKUP_FUNC(abort);
  LOOKUP_FUNC(interrupt);
  LOOKUP_FUNC(startingInstruction);
#undef LOOKUP_FUNC
}

int main(int argc, char** argv) {

  std::string binaryFilename = (argc >= 1 ? argv[argc-1]   : "" );
  std::vector<std::string> newArgv(argv,argv+argc);
  newArgv.push_back("-rose:output");
  newArgv.push_back(binaryFilename+"-binarySemantics.C");

  SgProject* proj = frontend(newArgv);
  
  ROSE_ASSERT (proj);
  SgSourceFile* newFile = isSgSourceFile(proj->get_fileList().front());
  ROSE_ASSERT(newFile != NULL);
  SgGlobal* g = newFile->get_globalScope();
  ROSE_ASSERT (g);

  //I am doing some experimental work to enable functions in the C representation
  //Set this flag to true in order to enable that work
  bool enable_functions = true;
  //Jeremiah did some work to enable a simplification and normalization of the 
  //C representation. Enable this work by setting this flag to true.
  bool enable_normalizations = false;

  vector<SgNode*> asmFiles = NodeQuery::querySubTree(proj, V_SgAsmGenericFile);
  ROSE_ASSERT (asmFiles.size() == 1);



  if( enable_functions == false)
  {
    //Representation of C normalizations withotu functions
    SgFunctionDeclaration* decl = buildDefiningFunctionDeclaration("run", SgTypeVoid::createType(), buildFunctionParameterList(), g);
    appendStatement(decl, g);
    SgBasicBlock* body = decl->get_definition()->get_body();
    //  ROSE_ASSERT(isSgAsmFile(asmFiles[0]));
    //  X86CTranslationPolicy policy(newFile, isSgAsmFile(asmFiles[0]));
    X86CTranslationPolicy policy(newFile, isSgAsmGenericFile(asmFiles[0]));
    ROSE_ASSERT( isSgAsmGenericFile(asmFiles[0]) != NULL);

    policy.switchBody = buildBasicBlock();
    removeDeadStores(policy.switchBody,policy);

    SgSwitchStatement* sw = buildSwitchStatement(buildVarRefExp(policy.ipSym), policy.switchBody);
    ROSE_ASSERT(isSgBasicBlock(sw->get_body()));

    SgWhileStmt* whileStmt = buildWhileStmt(buildBoolValExp(true), sw);

    appendStatement(whileStmt, body);
    policy.whileBody = sw;

    X86InstructionSemantics<X86CTranslationPolicy, WordWithExpression> t(policy);
    //AS FIXME: This query gets noting in the form in the repository. Doing this hack since we only 
    //have one binary file anyways.
    //vector<SgNode*> instructions = NodeQuery::querySubTree(asmFiles[0], V_SgAsmx86Instruction);
    vector<SgNode*> instructions = NodeQuery::querySubTree(proj, V_SgAsmx86Instruction);

    std::cout << "Instruction\n";
    for (size_t i = 0; i < instructions.size(); ++i) {
      SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
      ROSE_ASSERT (insn);
      try {
          t.processInstruction(insn);
      } catch (const X86InstructionSemantics<X86CTranslationPolicy, WordWithExpression>::Exception &e) {
          std::cout <<e.mesg <<": " <<unparseInstructionWithAddress(e.insn) <<"\n";
      }
    }


    if ( enable_normalizations == true )
    {
      //Enable normalizations of C representation
      //This is done heuristically where some steps
      //are repeated. It is not clear which order is 
      //the best
      {
        plugInAllConstVarDefs(policy.switchBody,policy) ;
        simplifyAllExpressions(policy.switchBody);
        removeIfConstants(policy.switchBody);
        removeDeadStores(policy.switchBody,policy);
        removeUnusedVariables(policy.switchBody);
      }
      {
        plugInAllConstVarDefs(policy.switchBody,policy) ;
        simplifyAllExpressions(policy.switchBody);
        removeIfConstants(policy.switchBody);
        removeDeadStores(policy.switchBody,policy);
      }
      removeUnusedVariables(policy.switchBody);
    }

  
  }else{ //Experimental changes to introduce functions into the C representation


    //When trying to add function I get that symbols are not defined

    //Iterate over the functions separately
    vector<SgNode*> asmFunctions = NodeQuery::querySubTree(proj, V_SgAsmFunctionDeclaration);

    for(size_t j = 0; j < asmFunctions.size(); j++ )
    {
      SgAsmFunctionDeclaration* binFunc = isSgAsmFunctionDeclaration( asmFunctions[j] );

      // Some functions (probably just one) are generated to hold basic blocks that could not
      // be assigned to a particular function. This happens when the Disassembler is overzealous
      // and the Partitioner cannot statically determine where the block belongs.  The name of
      // one such function is "***uncategorized blocks***".  [matzke 2010-06-29]
      if ((binFunc->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS))
        continue;

      //Some functions may be unnamed so we need to generate a name for those
      std::string funcName;
      if (binFunc->get_name().size()==0) {
	char addr_str[64];
	sprintf(addr_str, "0x%"PRIx64, binFunc->get_statementList()[0]->get_address());
	funcName = std::string("my_") + addr_str;;
      } else {
	funcName = "my" + binFunc->get_name();
      }

      //Functions can have illegal characters in their name. Need to replace those characters
      for ( int i = 0 ; i < funcName.size(); i++ )
      {
	char& currentCharacter = funcName.at(i);
	if ( currentCharacter == '.' )
	  currentCharacter = '_';
      }


      SgFunctionDeclaration* decl = buildDefiningFunctionDeclaration(funcName, SgTypeVoid::createType(), buildFunctionParameterList(), g);

      appendStatement(decl, g);
      SgBasicBlock* body = decl->get_definition()->get_body();
      X86CTranslationPolicy policy(newFile, isSgAsmGenericFile(asmFiles[0]));
      ROSE_ASSERT( isSgAsmGenericFile(asmFiles[0]) != NULL);
      policy.switchBody = buildBasicBlock();
      SgSwitchStatement* sw = buildSwitchStatement(buildVarRefExp(policy.ipSym), policy.switchBody);
      SgWhileStmt* whileStmt = buildWhileStmt(buildBoolValExp(true), sw);
      appendStatement(whileStmt, body);
      policy.whileBody = sw;
      X86InstructionSemantics<X86CTranslationPolicy, WordWithExpression> t(policy);
      vector<SgNode*> instructions = NodeQuery::querySubTree(binFunc, V_SgAsmx86Instruction);

      for (size_t i = 0; i < instructions.size(); ++i) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
	if( insn->get_kind() == x86_nop )
	  continue;
        ROSE_ASSERT (insn);
        try {
            t.processInstruction(insn);
        } catch (const X86InstructionSemantics<X86CTranslationPolicy, WordWithExpression>::Exception &e) {
            std::cout <<e.mesg <<": " <<unparseInstructionWithAddress(e.insn) <<"\n";
        }
      }

    }

    //addDirectJumpsToSwitchCases(policy);


  }

  proj->get_fileList().erase(proj->get_fileList().end() - 1); // Remove binary file before calling backend

//  AstTests::runAllTests(proj);

  //Compile the resulting project

  return backend(proj);
}
