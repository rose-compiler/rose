#include "rose.h"
#include "createCloneDetectionVectors.h"

using namespace std;

SynthesizedAttribute::SynthesizedAttribute() {
  nodesInSubtree = new int[V_SgNumVariants];
  for( int i = 0; i < V_SgNumVariants; ++i)
  {
	(nodesInSubtree)[i] = 0;
  }


};

void SynthesizedAttribute::addToVector(int *vectorToAdd){
  int j=0;
  for( int i = 0; i < V_SgNumVariants; i++)
  {
	(nodesInSubtree)[i] += (vectorToAdd)[i];
	j+=(nodesInSubtree)[i];
	//std::cout << (*nodesInSubtree)[i] << " " ;

  }
  //std::cout << std::endl << std::endl;
  //std::cout <<  j << std::endl;
}

SynthesizedAttribute& SynthesizedAttribute::operator=( const SynthesizedAttribute& rhs ){
  nodesInSubtree = rhs.nodesInSubtree;
  return (*this);
}



SynthesizedAttribute
CreateCloneDetectionVectors::evaluateSynthesizedAttribute (
     SgNode* astNode,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
	 static int currentStride = 0;

     SynthesizedAttribute returnAttribute;
     //test.push_back(astNode->class_name());
     //initialize to zero
     for( int i = 0; i < V_SgNumVariants; ++i)
        {
         (returnAttribute.nodesInSubtree)[i] = 0;
        }

     nodeToCloneVector[astNode] = returnAttribute.nodesInSubtree;

     //Add current node to vector
     (returnAttribute.nodesInSubtree)[astNode->variantT()]+=1;


     //Add the vectors in the subtrees of the vector to this vector
     for(SubTreeSynthesizedAttributes::iterator iItr = synthesizedAttributeList.begin();
         iItr != synthesizedAttributeList.end(); ++iItr){
#if 0 
                for( int i = 0; i < lengthVariantT; i++)
                  {
                     std::cout << (iItr->nodesInSubtree)[i] << " " ;
            
                  }
                std::cout << std::endl << std::endl;
#endif

         returnAttribute.addToVector ( (*iItr).nodesInSubtree );
     }

     //std::cout << "\n\nVector of names" << std::endl;

#if 0	 
	 //Print out the number of elements found for each variant in the subtree
     for(int i = 0;  i < lengthVariantT; i++  ){
       if(returnAttribute.nodesInSubtree[i] > 0)
          std::cout << returnAttribute.nodesInSubtree[i] << " " << variantToName[i] << " ";
	 }
     std::cout << "\n\n";
#endif

	 //Write to the file specified in the config file on the commandline
	 if(std::find(variantToWriteToFile.begin(), variantToWriteToFile.end(), astNode->variantT()) != variantToWriteToFile.end())
	 {

	   int numTokens = 0;

	   for(unsigned int i=0; i < variantNumVec.size(); i++ )
	   {
		 numTokens+= returnAttribute.nodesInSubtree[i];
	   }


	   if( numTokens >= minTokens )
	   {

		 currentStride++;

		 //To implement the concept of a stride we need to not overcount
		 //subtrees. But I am not sure if the vectors which contains this vector
		 //should have a smaller


		 for(unsigned int i = 0;  i < variantNumVec.size(); i++  ){
		   mergedVector[i]+= returnAttribute.nodesInSubtree[i];
		 }

		 if( currentStride >= stride ){

		   VariantVector stmts(V_SgStatement);
		   VariantVector exprs(V_SgExpression);
		   VariantVector decls(V_SgDeclarationStatement);

		   int n_stmts = 0;
		   int n_exprs = 0;
		   int n_decls = 0;

		   for( VariantVector::iterator iItr = stmts.begin(); 
			   iItr != stmts.end(); ++iItr )
			 n_stmts+=stmts[*iItr];

		   for( VariantVector::iterator iItr = exprs.begin(); 
			   iItr != exprs.end(); ++iItr )
			 n_exprs+=exprs[*iItr];

		   for( VariantVector::iterator iItr = decls.begin(); 
			   iItr != decls.end(); ++iItr )
			 n_decls+=decls[*iItr];


		   myfile << "# FILE:" <<astNode->get_file_info()->get_filenameString();
		   myfile << ", LINE:" <<astNode->get_file_info()->get_line();
		   myfile << ", OFFSET:" <<astNode->get_file_info()->get_col();
		   myfile << ", NODE_KIND:" << astNode->variantT();
		   myfile << ", CONTEXT_KIND:0, NEIGHBOR_KIND:0";
           myfile << ", NUM_NODE:" << numTokens;
		   myfile << ", NUM_DECL:" << n_decls;
		   myfile << ", NUM_STMT:" << n_stmts;
		   myfile << ", NUM_EXPR:" << n_exprs;
		   myfile << ", TBID:0, TEID:0, VARs:{}0,"; 
		   myfile << std::endl;

		   for(unsigned int i = 0;  i < variantNumVec.size(); i++  ){
			 myfile << mergedVector[i] << " ";

			 //std::cout << returnAttribute.nodesInSubtree[variantNumVec[i]] << " " << roseGlobalVariantNameList[variantNumVec[i]] << " ";
		   }
		   myfile << std::endl;

		   currentStride = 0;

		   for(unsigned int i = 0;  i < variantNumVec.size(); i++  ){
			 mergedVector[i] = 0;
		   }


		 }
	   }

	 }

     return returnAttribute;
   }
