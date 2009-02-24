
#include <string>
#include <fstream>
#include <iostream>
#include <fstream>


#ifndef CREATE_CLONE_DETECTION_VECTORS
#define CREATE_CLONE_DETECTION_VECTORS

class SynthesizedAttribute
   {
     public:
         //Nodes In the Subtree of this node. Currently does not contain
         //the node itself
         int *nodesInSubtree;


         SynthesizedAttribute(); 
         void addToVector(int *vectorToAdd);
     
        SynthesizedAttribute & operator=( const SynthesizedAttribute& rhs );
            
   };

class CreateCloneDetectionVectors : public SgBottomUpProcessing<SynthesizedAttribute>
   {
     public:

	   //The file to write to
	   std::ofstream myfile;


       std::map<SgNode*, int* > nodeToCloneVector; 

	   //Variants to put in the generated vector
       std::vector<int> variantNumVec;

	   //Variants to write to file
	   std::vector<int> variantToWriteToFile;

	   //stride
	   int stride;

	   //min number of tokens before writing out
	   int minTokens;


	   //Current merged vector
	   std::vector<int> mergedVector;

       // Functions required
       SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             SubTreeSynthesizedAttributes synthesizedAttributeList );

   };




#endif
