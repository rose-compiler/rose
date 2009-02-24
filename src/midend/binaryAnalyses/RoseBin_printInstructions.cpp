/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/
#include "rose.h"
#include "RoseBin_printInstructions.h"

using namespace std;

/****************************************************
 * Assign the global node and the filename
 ****************************************************/
void RoseBin_printInstructions::initPrint(SgAsmNode* root, char* fileName) {
  globalNode = root;
  filename = fileName;
  myfile.open(filename);
}

/****************************************************
 * unparse the binary AST
 ****************************************************/
void RoseBin_printInstructions::unparsePrint() {
  traverse(globalNode, preorder);
  closeFile();
}

/****************************************************
 * unparse binary instruction
 ****************************************************/
void RoseBin_printInstructions::visit(SgNode* n) {
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);

//std::string str=binInst->get_raw_bytes();
//char *array=strdup(str.c_str());
  SgUnsignedCharList array = binInst->get_raw_bytes();

  int size=0;
  string hexcode ="";
//for(int i=0; array[i]; ++i) {
  for(size_t i=0; i < array.size(); ++i) {
    size++;
    //    std::cerr << std::bitset<sizeof(char)*8>((int)array[i]);
    const char* table = "0123456789abcdef";
 // unsigned char c = (char)array[i];
    unsigned char c = array[i];
    string S;
    S = table[c>>4];
    S += table[c & 0x0F];
    hexcode = hexcode + " "+S;
  }
  //  std::cerr << " size: " << size << endl; 

  unsigned int address = binInst->get_address();
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string address_str = addrhex.str();
  // print the instruction
  myfile << address_str << "  " << size << "   " << hexcode << "\n";
}

/****************************************************
 * close the file
 ****************************************************/
void RoseBin_printInstructions::closeFile() {
  ROSE_ASSERT(myfile);
  myfile.close();  
}

