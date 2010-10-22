#include "rose.h"
#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>


#include <iostream>
#include <list>


using namespace std;
using namespace boost;
class DeleteAST : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

void
DeleteAST::visit(SgNode* node)
   {
     delete node;
   }

void DeleteSgTree( SgNode* root)
{
  DeleteAST deleteTree;
  deleteTree.traverse(root,postorder);
}

SgNode* disassembleFile(std::string disassembleName)
{

    RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
    RoseBin_Arch::arch=RoseBin_Arch::bit32;
    RoseBin_OS::os_sys=RoseBin_OS::linux_op;
    RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;


    RoseFile* roseBin = new RoseFile( (char*)disassembleName.c_str()  );

    cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
    // query the DB to retrieve all data

    SgNode* globalBlock = roseBin->retrieve_DB();

    // traverse the AST and test it
    roseBin->test();


    return globalBlock;

}


static string htmlEscape(const string& s) {
  string s2;
  for (size_t i = 0; i < s.size(); ++i) {
    switch (s[i]) {
      case '<': s2 += "&lt;"; break;
      case '>': s2 += "&gt;"; break;
      case '&': s2 += "&amp;"; break;
      default: s2 += s[i]; break;
    }
  }
  return s2;
}


enum ExpressionCategory {ec_reg = 0, ec_mem = 1, ec_val = 2};

static const size_t numberOfInstructionKinds = x86_last_instruction;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->get_kind();}



inline ExpressionCategory getCategory(SgAsmExpression* e) {
  if (isSgAsmValueExpression(e)) {
    return ec_val;
  } else if (isSgAsmRegisterReferenceExpression(e)) {
    return ec_reg;
  } else if (isSgAsmMemoryReferenceExpression(e)) {
    return ec_mem;
  } else {
    abort();
  }
}

SgAsmExpressionPtrList& getOperands(SgAsmInstruction* insn) {
  SgAsmOperandList* ol = insn->get_operandList();
  SgAsmExpressionPtrList& operands = ol->get_operands();
  return operands;
}
static map<string, void*> internTable;

inline void* intern(const std::string& s) {
  map<string, void*>::const_iterator i = internTable.find(s);
  if (i == internTable.end()) {
    void* sCopy = new string(s);
    internTable.insert(std::make_pair(s, sCopy));
    return sCopy;
  } else {
    return i->second;
  }
}
static map<SgAsmExpression*, void*> unparseAndInternTable;

inline void* unparseAndIntern(SgAsmExpression* e) {
  map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
  if (i == unparseAndInternTable.end()) {
    void* sPtr = intern(unparseExpression(e));
    unparseAndInternTable.insert(std::make_pair(e, sPtr));
    return sPtr;
  } else {
    return i->second;
  }
}
 
void numberOperands(std::vector<SgAsmx86Instruction*>::iterator beg,
        std::vector<SgAsmx86Instruction*>::iterator end, map<SgAsmExpression*, size_t> numbers[3]) {
  map<void*, size_t> stringNumbers[3];
  for (; beg != end; ++beg) {
    SgAsmx86Instruction* insn = *beg;
    const SgAsmExpressionPtrList& operands = getOperands(insn);
    //size_t operandCount = operands.size();
    for (size_t j = 0; j < operands.size(); ++j) {
      SgAsmExpression* e = operands[j];
      ExpressionCategory cat = getCategory(e);
      void* str = unparseAndIntern(e);
      map<void*, size_t>& currentStringNums = stringNumbers[(int)cat];
      map<void*, size_t>::const_iterator stringNumIter = currentStringNums.find(str);
      size_t num = (stringNumIter == currentStringNums.end() ? currentStringNums.size() : stringNumIter->second);
      if (stringNumIter == currentStringNums.end()) currentStringNums.insert(std::make_pair(str, num));
      numbers[(int)cat][e] = num;
    }
  }
}
std::string normalizeInstructionsToHTML(std::vector<SgAsmx86Instruction*>::iterator beg, 
    std::vector<SgAsmx86Instruction*>::iterator end)
{
    string normalizedUnparsedInstructions;
    map<SgAsmExpression*, size_t> valueNumbers[3];
    numberOperands( beg,end, valueNumbers);

    // Unparse the normalized forms of the instructions
    for (; beg != end; ++beg ) {
      SgAsmx86Instruction* insn = *beg;
      string mne = insn->get_mnemonic();
      boost::to_lower(mne);
      mne = "<font color=\"red\">" + htmlEscape(mne)+"</font>";

      normalizedUnparsedInstructions += mne;
      const SgAsmExpressionPtrList& operands = getOperands(insn);
      // Add to total for this variant
      // Add to total for each kind of operand
      size_t operandCount = operands.size();

      normalizedUnparsedInstructions += "<font color=\"blue\">";
      for (size_t i = 0; i < operandCount; ++i) {
        SgAsmExpression* operand = operands[i];
        ExpressionCategory cat = getCategory(operand);
        map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
        assert (numIter != valueNumbers[(int)cat].end());
        size_t num = numIter->second;

        normalizedUnparsedInstructions += (cat == ec_reg ? " R" : cat == ec_mem ? " M" : " V") + boost::lexical_cast<string>(num);
      }
      normalizedUnparsedInstructions += "; </font> <br> ";
  
    }
   
    return normalizedUnparsedInstructions;
};
void normalizeInstructionInSubTree(SgNode* topNode ){
  vector<SgAsmx86Instruction*> insns;
  FindInstructionsVisitor vis;
  AstQueryNamespace::querySubTree(topNode, std::bind2nd( vis, &insns ));

  for(std::vector<SgAsmx86Instruction*>::iterator iItr = insns.begin(); iItr !=  insns.end();
      ++iItr)
  {
      SgAsmx86Instruction* insn = *iItr;
       SgAsmExpressionPtrList& operands = getOperands(insn);
      // Add to total for this variant
      // Add to total for each kind of operand
      size_t operandCount = operands.size();

      for (size_t i = 0; i < operandCount; ++i) {
        SgAsmExpression* operand = operands[i];
        SgAsmExpression* newOperand;
        if( isSgAsmMemoryReferenceExpression(operand) )
        {
          SgAsmMemoryReferenceExpression* memRefExp = new SgAsmMemoryReferenceExpression;
          SgAsmWordValueExpression* wordVal = new SgAsmWordValueExpression;
          wordVal->set_value(0);

          memRefExp->set_segment(wordVal);
          memRefExp->set_address(wordVal);
          memRefExp->set_type(new SgAsmTypeWord);
          newOperand = memRefExp;
        }else if(isSgAsmRegisterReferenceExpression(operand) ){
          SgAsmx86RegisterReferenceExpression* regRef = new SgAsmx86RegisterReferenceExpression;
          regRef->get_descriptor().set_major(x86_regclass_mm);
          regRef->get_descriptor().set_minor(0);

          newOperand = regRef;

        }else{
          SgAsmWordValueExpression* wordVal = new SgAsmWordValueExpression;
          wordVal->set_value(0);
          newOperand = wordVal;
          
        }

        newOperand->set_parent(operand->get_parent());

        DeleteSgTree(operands[i]);
        operands[i]=newOperand;
      }
        //std::cout << "Unparsed: " <<unparseX86Instruction(insn)<< std::endl;

  }







};





int main(int argc, char** argv)
{

//  SgNode* disFile = disassembleFile(argv[1]);
  SgProject* disFile = frontend(argc,argv);

#if 0
  VariantVector vv1(V_SgAsmPEDLL);
  vv1.push_back(V_SgAsmPEDLL); 
  std::vector<SgNode*> peDLL = NodeQuery::querySubTree(disFile,vv1);

  std::cout << "size: " << peDLL.size();
    std::cout << "BEGIN - PEDLL" <<std::endl;

  for(int i=0; i < peDLL.size() ; i++)
  {
    SgAddressList addrList = isSgAsmPEDLL(peDLL[i])->get_hintname_rvas();
    for(int j = 0; i < addrList.size() ; j++ )
    {
    std::cout << addrList[j] << " ";

    }
      

  };
#endif
  
  std::cout << "END - PEDLL" <<std::endl;
 
  {
//  normalizeInstructionInSubTree(disFile);
  
  FindInstructionsVisitor vis;

  //  backend((SgProject*)disFile);
  vector<SgNode*> insnsA;

  

  AstQueryNamespace::querySubTree(disFile, std::bind2nd( vis, &insnsA ));

  if(insnsA.size() == 0)
  {
    std::cerr << "error: there are no instrucitons" << std::endl;
    return 0;
  }


  {
    ofstream myfile;
    std::string outputFile = string(argv[1])+".roseTxt";
    myfile.open (outputFile.c_str());

    std::vector<SgAsmx86Instruction*> instructions;
    for(int i = 0; i < insnsA.size(); i++ )
      if(isSgAsmx86Instruction(insnsA[i]))
      {
        if(isSgAsmx86Instruction(insnsA[i])->get_kind() != x86_call ) 
          normalizeInstructionInSubTree(insnsA[i]);
        myfile <<   unparseInstruction((SgAsmInstruction*)insnsA[i]) << "\n";
      }
    myfile.close();
  }

  {
    ofstream myfile;
    std::string outputFile = string(argv[1])+".roseTxtNoEndl";
    myfile.open (outputFile.c_str());

    std::vector<SgAsmx86Instruction*> instructions;
    for(int i = 0; i < insnsA.size(); i++ )
      if(isSgAsmx86Instruction(insnsA[i]))
      {
  //      normalizeInstructionInSubTree(insnsA[i]);
    
        myfile <<   unparseInstruction((SgAsmInstruction*)insnsA[i]) ;
      }

    myfile.close();

  }

 //     instructions.push_back(isSgAsmx86Instruction(insnsA[i]));


//  myfile << normalizeInstructionsToHTML(instructions.begin(), instructions.end());


  exit(0);

  }
};
