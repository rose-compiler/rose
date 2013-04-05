#include "rose.h"
#include <string>
#include <fstream>
#include <iostream>
#include <fstream>
#include <stack>
#include <deque>

#ifndef CREATE_CLONE_DETECTION_VECTORS
#define CREATE_CLONE_DETECTION_VECTORS

#include <stdio.h>
#include <iostream>
#include "rose.h"

#include "sqlite3x.h"

#include "ProcessDatabase.hpp"
#include "ReadOptions.hpp"
//stack of vectors which has been free'ed
extern std::stack< std::vector<int> * > vecStack;
extern int cloneVectorSize;


class SynthesizedAttribute
   {
     public:
         //Nodes In the Subtree of this node. Currently does not contain
         //the node itself
	   std::vector<int>* nodesInSubtree;
       SgNode* astNode;

	   SynthesizedAttribute(); 

         void addToVector(std::vector<int>* vectorToAdd);
     
        SynthesizedAttribute & operator=( const SynthesizedAttribute& rhs );
            
   };

//Free a vector. (Put it on Stack)
void freeVector(std::vector<int>* v);

 //Create a new Vector. guaranteed that all values are zero.
std::vector<int>* newZeroValVector();

#if 0
class CreateCloneDetectionVectors : public SgBottomUpProcessing<SynthesizedAttribute>, RoseBin_DB
#else
class CreateCloneDetectionVectors : public ProcessDatabase::OperateBase, public SgBottomUpProcessing<SynthesizedAttribute>
#endif
   {

	 public:
	 enum type_of_tree {
		 source,
		 binary
	   };
	 int inVecHash;
	 int toFileHash;

	 private:

	   std::vector<std::string> registerNames;
	   int registerCounter;

	   VariantVector stmts;
       VariantVector exprs;
       VariantVector decls;

	   //The file to write to
	   std::ofstream myfile;
       //The file to write the vectors as text to
	   std::ofstream myCloneFile;


	   bool addRegistersToOutput;
	   
	   //This feature is currently disabled
       //std::map<SgNode*, std::vector<int>* > nodeToCloneVector; 
	   //stride
	   int stride;

	   //min number of tokens before writing out
	   int minTokens;

	   //Environment variable to output only whole blocks
	   bool outputOnlyWholeBlocks;

	   //Environment variable to respect procedure boundaries
	   bool ignoreProcedureBoundaries;

	   //Type of tree that we are traversing
       type_of_tree treeType;

	   //The SQL database name if a binary tree
	   std::string database_name;

	   //Variants to put in the generated vector
       std::vector<int> variantNumVec;

	   //Variants to write to file
	   std::vector<int> variantToWriteToFile;

	   //Current merged vector
       std::vector<int>* mergedVector;

	   //Set current stride
	   int currentStride;

	  
	   VariantT opNum;

	   //Holds the current elements in the vector
	   std::deque< std::pair< std::vector<int>*, SgNode*>  >  window;

	   //window-size
       int windowSize;

       std::string filename;

	      public:

	   	
	   CreateCloneDetectionVectors(bool addRegToOut, type_of_tree, ReadOptions&,int,int, int);

  	   ~CreateCloneDetectionVectors();
  
       //Write a vector to FILE

       // Functions required
       SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
			 SubTreeSynthesizedAttributes synthesizedAttributeList );

	  
       void writeVectorToFile( const SgNode* beginNode, const SgNode* endNode, const std::vector<int>* v, const int numTokens,
		   int, std::string stringToUnparse );

       void writeUnparseStringToFile( const SgNode* beginNode, const SgNode* endNode, const std::vector<int>* mergedVector, const int numTokens , int bin_offset, std::string stringToUnparse);
       //	   void resolveOperand(SgAsmExpression* expr, std::string *type,
       //			       RoseBin_DataFlowAbstract* dfa);
       //  void resolveOperand(SgAsmExpression* expr, std::string *type);

	   std::string unparseInstruction(SgAsmInstruction* binInst, int side);
	   std::string resolveExpression(SgAsmExpression* expr);
	   uint64_t getValueInExpression(SgAsmValueExpression* valExp);

       bool encode;
	   
	   std::vector<std::pair<X86RegisterClass, int> >  regVec;
	   std::vector<std::string> memRefVec;
	   std::vector<uint64_t>    valVec;

	   void traverse(SgNode* globalBlock);
   };

//Function to initialize and run the clone detection
void runCloneDetection(std::vector<std::string> argvList, SgNode* globalBlock, CreateCloneDetectionVectors::type_of_tree,
	ReadOptions& options );




#endif
