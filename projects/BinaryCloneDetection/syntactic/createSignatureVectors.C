#include "rose.h"
#include "SqlDatabase.h"

#include "createSignatureVectors.h"
#include "vectorCompression.h"
#include "../semantic/CloneDetectionLib.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <ext/hash_map>
#include <openssl/md5.h>          //FIXME: use libgcrypt instead [Robb P. Matzke 2013-08-12]
#include <stdio.h>
#include <string.h>


using namespace std;
using namespace sqlite3x;
using namespace __gnu_cxx;

namespace __gnu_cxx {
template <>
struct hash<SgAsmExpression*> {
    size_t operator()(SgAsmExpression* expr) const {
        return size_t(expr);
    }
};

template <>
struct hash<void*> {
    size_t operator()(void* expr) const {
        return size_t(expr);
    }
};
}

#define NORMALIZED_UNPARSED_INSTRUCTIONS

#if 0
static const size_t numberOfInstructionKinds = x86_last_instruction;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->get_kind();}
#else
static const size_t numberOfInstructionKinds = V_SgNumVariants;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->variantT();}
#endif

size_t numVectorsGenerated = 0;

static map<string, void*> internTable;

inline void*
intern(const std::string& s)
{
    map<string, void*>::iterator i = internTable.find(s);
    if  (i == internTable.end()) {
        void* sCopy = new string(s);
        internTable.insert(std::make_pair(s, sCopy));
        return sCopy;
    } else {
        return i->second;
    }
}

static hash_map<SgAsmExpression*, void*> unparseAndInternTable;

inline void*
unparseAndIntern(SgAsmExpression* e)
{
    hash_map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
    if (i == unparseAndInternTable.end()) {
        void* sPtr = intern(unparseX86Expression(e, NULL, NULL));
        unparseAndInternTable.insert(std::make_pair(e, sPtr));
        return sPtr;
    } else {
        return i->second;
    }
}

inline ExpressionCategory
getCategory(SgAsmExpression* e)
{
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

SgAsmExpressionPtrList&
getOperands(SgAsmInstruction* insn)
{
    SgAsmOperandList* ol = insn->get_operandList();
    SgAsmExpressionPtrList& operands = ol->get_operands();
    return operands;
}

void
numberOperands(SgAsmx86Instruction* firstInsn[], size_t insnCount, hash_map<SgAsmExpression*, size_t> numbers[3])
{
    hash_map<void*, size_t> stringNumbers[3];
    for (size_t i = 0; i < insnCount; ++i) {
        SgAsmx86Instruction* insn = firstInsn[i];
        const SgAsmExpressionPtrList& operands = getOperands(insn);
        //size_t operandCount = operands.size();
        for (size_t j = 0; j < operands.size(); ++j) {
            SgAsmExpression* e = operands[j];
            ExpressionCategory cat = getCategory(e);
            void* str = unparseAndIntern(e);
            hash_map<void*, size_t>& currentStringNums = stringNumbers[(int)cat];
            hash_map<void*, size_t>::const_iterator stringNumIter = currentStringNums.find(str);
            size_t num = (stringNumIter == currentStringNums.end() ? currentStringNums.size() : stringNumIter->second);
            if (stringNumIter == currentStringNums.end())
                currentStringNums.insert(std::make_pair(str, num));
            numbers[(int)cat][e] = num;
        }
    }
}

void addVectorToDatabase(const SqlDatabase::TransactionPtr&, const SignatureVector& vec, const std::string& functionName,
                         size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions,
                         SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride);

void addFunctionStatistics(const SqlDatabase::TransactionPtr&, const std::string& filename, const std::string& functionName,
                           size_t functionId, size_t numInstructions);

// Ignores function boundaries
bool
createVectorsForAllInstructions(SgNode* top, const std::string& filename, const std::string& functionName, int functionId,
                                size_t windowSize, size_t stride, const SqlDatabase::TransactionPtr &tx)
{
    bool retVal = false;
    vector<SgAsmx86Instruction*> insns;
    FindInstructionsVisitor vis;
    AstQueryNamespace::querySubTree(top, std::bind2nd( vis, &insns ));
    size_t insnCount = insns.size();

    for (size_t windowStart = 0; windowStart + windowSize <= insnCount; windowStart += stride) {
        static SignatureVector vec;
        vec.clear();
        hash_map<SgAsmExpression*, size_t> valueNumbers[3];
        numberOperands(&insns[windowStart], windowSize, valueNumbers);
        string normalizedUnparsedInstructions;
        // Unparse the normalized forms of the instructions
        for (size_t insnNumber = 0; insnNumber < windowSize; ++insnNumber) {
            SgAsmx86Instruction* insn = insns[windowStart + insnNumber];
            size_t var = getInstructionKind(insn);
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
            string mne = insn->get_mnemonic();
            boost::to_lower(mne);
            normalizedUnparsedInstructions += mne;
#endif
            const SgAsmExpressionPtrList& operands = getOperands(insn);
            size_t operandCount = operands.size();
            // Add to total for this variant
            ++vec.totalForVariant(var);
            // Add to total for each kind of operand
            for (size_t i = 0; i < operandCount; ++i) {
                SgAsmExpression* operand = operands[i];
                ExpressionCategory cat = getCategory(operand);
                ++vec.opsForVariant(cat, var);
                // Add to total for this unique operand number (for this window)
                hash_map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
                assert (numIter != valueNumbers[(int)cat].end());
                size_t num = numIter->second;
                ++vec.specificOp(cat, num);
                // Add to total for this kind of operand
                ++vec.operandTotal(cat);
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
                normalizedUnparsedInstructions += (cat == ec_reg ? "R" : cat == ec_mem ? "M" : "V") +
                                                  boost::lexical_cast<string>(num);
#endif
            }

            // Add to total for this pair of operand kinds
            if (operandCount >= 2) {
                ExpressionCategory cat1 = getCategory(operands[0]);
                ExpressionCategory cat2 = getCategory(operands[1]);
                ++vec.operandPair(cat1, cat2);
            }
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
            if (insnNumber + 1 < windowSize) {
                normalizedUnparsedInstructions += ";";
            }
#endif
        }

        // Add vector to database
        addVectorToDatabase(tx, vec, functionName, functionId, windowStart/stride, normalizedUnparsedInstructions,
                            &insns[windowStart], filename, windowSize, stride);
	retVal = true;
    }
    addFunctionStatistics(tx, filename, functionName, functionId, insnCount);
    return retVal;
}

void
createVectorsNotRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride,
                                             const SqlDatabase::TransactionPtr &tx)
{
    int functionId = tx->statement("select coalesce(max(id),-1)+1 from functions")->execute_int(); // zero origin
    std::string functionName = filename + "-all-instructions";
    createVectorsForAllInstructions(top, filename, functionName, functionId, windowSize, stride, tx);
    tx->statement("insert into functions(file, function_name) values (?,?)")
        ->bind(0, filename)
        ->bind(1, functionName)
        ->execute();
    cout << "Total vectors generated: " << numVectorsGenerated << endl;
}

void
createVectorsRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride,
                                          const SqlDatabase::TransactionPtr &tx)
{
    struct InstructionSelector: SgAsmFunction::NodeSelector {
        virtual bool operator()(SgNode *node) {
            return isSgAsmInstruction(node)!=NULL;
        }
    } iselector;

    struct DataSelector: SgAsmFunction::NodeSelector {
        virtual bool operator()(SgNode *node) {
            return isSgAsmStaticData(node)!=NULL;
        }
    } dselector;

    SqlDatabase::StatementPtr cmd1 = tx->statement("insert into functions"
                                                   // 0   1     2              3         4      5      6
                                                   " (id, file, function_name, entry_va, isize, dsize, size)"
                                                   " values(?,?,?,?,?,?,?)");
    
    SqlDatabase::StatementPtr cmd2 = tx->statement("insert into instructions"
                                                   // 0        1     2            3                      4
                                                   " (address, size, function_id, index_within_function, assembly)"
                                                   " values (?,?,?,?,?)");

    vector<SgAsmFunction*> funcs = SageInterface::querySubTree<SgAsmFunction>(top);
    int functionId = tx->statement("select coalesce(max(id),-1)+1 from functions")->execute_int(); // zero origin
    
    for (vector<SgAsmFunction*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi, ++functionId) {
        ExtentMap e_insns, e_data, e_total;
        (*fi)->get_extent(&e_insns, NULL, NULL, &iselector);
        (*fi)->get_extent(&e_data,  NULL, NULL, &dselector);
        (*fi)->get_extent(&e_total);

	createVectorsForAllInstructions(*fi, filename, (*fi)->get_name(), functionId, windowSize, stride, tx);
        cmd1->bind(0, functionId);
        cmd1->bind(1, filename);
        cmd1->bind(2, (*fi)->get_name() );
        cmd1->bind(3, (*fi)->get_entry_va());
        cmd1->bind(4, e_insns.size());
        cmd1->bind(5, e_data.size());
        cmd1->bind(6, e_total.size());
        cmd1->execute();

        vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(*fi);
        for (size_t i=0; i<insns.size(); ++i) {
            cmd2->bind(0, insns[i]->get_address());
            cmd2->bind(1, insns[i]->get_size());
            cmd2->bind(2, functionId);
            cmd2->bind(3, i);
            cmd2->bind(4, unparseInstructionWithAddress(insns[i]));
            cmd2->execute();
        }
    }
    cerr << "Total vectors generated: " << numVectorsGenerated << endl;
}

void
addFunctionStatistics(const SqlDatabase::TransactionPtr &tx, const std::string& filename, const std::string& functionName,
                      size_t functionId, size_t numInstructions)
{
    tx->statement("insert into function_statistics (function_id, num_instructions) values (?,?)")
        ->bind(0, functionId)
        ->bind(1, numInstructions)
        ->execute();
}






void
addVectorToDatabase(const SqlDatabase::TransactionPtr &tx, const SignatureVector& vec, const std::string& functionName,
                    size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions,
                    SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride)
{
    ++numVectorsGenerated;

    vector<uint8_t> compressedCounts = compressVector(vec.getBase(), SignatureVector::Size);
    size_t vectorSum = 0;
    for (size_t i=0; i<SignatureVector::Size; ++i)
        vectorSum += vec[i];

    ExtentMap extent;
    for (size_t i=0; i<windowSize; ++i)
        extent.insert(Extent(firstInsn[i]->get_address(), firstInsn[i]->get_size()));

    unsigned char md[16];
    MD5((const unsigned char*)normalizedUnparsedInstructions.data(), normalizedUnparsedInstructions.size(), md);

    SqlDatabase::StatementPtr cmd = tx->statement("insert into vectors"
                                                  // 0   1            2                      3     4             5
                                                  " (id, function_id, index_within_function, line, last_insn_va, size,"
                                                  // 6            7           8
                                                  "sum_of_counts, counts_b64, instr_seq_b64)"
                                                  " values (?,?,?,?,?,?,?,?,?)");
    int vector_id = tx->statement("select coalesce(max(id),0)+1 from vectors")->execute_int(); // 1-origin
    cmd->bind(0, vector_id);
    cmd->bind(1, functionId);
    cmd->bind(2, indexWithinFunction);
    cmd->bind(3, firstInsn[0]->get_address());
    cmd->bind(4, firstInsn[windowSize-1]->get_address());
    cmd->bind(5, extent.size());
    cmd->bind(6, vectorSum);
    cmd->bind(7, StringUtility::encode_base64(&compressedCounts[0], compressedCounts.size()));
    cmd->bind(8, StringUtility::encode_base64(md, 16));
    cmd->execute();
}

void
dropDatabases(const SqlDatabase::TransactionPtr &tx)
{
    extern const char *syntactic_schema_drop; // defined in machine-generated SyntacticSchema.C
    tx->execute(syntactic_schema_drop);
}

void
createDatabases(const SqlDatabase::TransactionPtr &tx)
{
    extern const char *syntactic_schema_create; // defined in machine-generated SyntacticSchema.C
    tx->execute(syntactic_schema_create);
}

/* ========================================================================================================= 
       SOURCE CODE CLONE DETECTION

*/

void
addSourceVectorToDatabase(const SqlDatabase::TransactionPtr &tx, const SignatureVector& vec,
                    size_t functionId, SgLocatedNode* firstNode, SgLocatedNode* lastNode)
{
    ++numVectorsGenerated;

    vector<uint8_t> compressedCounts = compressVector(vec.getBase(), SignatureVector::Size);
    size_t vectorSum = 0;
    for (size_t i=0; i<SignatureVector::Size; ++i)
        vectorSum += vec[i];

    SqlDatabase::StatementPtr cmd = tx->statement("insert into vectors"
                                                  // 0   1            2                      3     4             5
                                                  " (id, function_id, index_within_function, line, last_insn_va, size,"
                                                  // 6            7           8
                                                  "sum_of_counts, counts_b64, instr_seq_b64)"
                                                  " values (?,?,?,?,?,?,?,?,?)");
    int vector_id = tx->statement("select coalesce(max(id),0)+1 from vectors")->execute_int(); // 1-origin
    
    std::string base64_encoding = StringUtility::encode_base64(&compressedCounts[0], compressedCounts.size());

    unsigned char md[16];
    MD5((const unsigned char*)&compressedCounts[0], compressedCounts.size(), md);


    int start_line = isSgLocatedNode(firstNode) ? firstNode->get_file_info()->get_line() : 0;
    int end_line   = isSgLocatedNode(lastNode)  ? lastNode->get_file_info()->get_line()  : 0;

    cmd->bind(0, vector_id);
    cmd->bind(1, functionId);
    cmd->bind(2, start_line );
    cmd->bind(3, start_line );
    cmd->bind(4, end_line );
    // this is the size of the code - don't know if that makes as much sense for source so I am using vectorSum 
    cmd->bind(5, vectorSum); 
    cmd->bind(6, vectorSum);
    cmd->bind(7, base64_encoding );
    // In source we do not create a normalized string representation as we do for source. 
    //   We therefore just use the encoding of the compressedCounts 
    cmd->bind(8,  StringUtility::encode_base64(md, 16) );     cmd->execute();
}



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

//Current merged vector


class GenerateMergedVector{
     const SqlDatabase::TransactionPtr tx;
 
     unsigned int stride;
     unsigned int windowSize;
     unsigned int functionId;

     unsigned int currentVectorId;
     unsigned int currentStride;

     std::vector< std::pair<SignatureVector*, SgLocatedNode*> > mergedVectorArray;
     std::vector<int> variantNumVec;
     SignatureVector mergedVector;

   public: 
     
    GenerateMergedVector(const SqlDatabase::TransactionPtr &tmp_tx, int _stride, int _windowSize, int _functionId,  
      std::vector<int> _variantNumVec);

    std::pair<SignatureVector*, SgLocatedNode*>&
    getSignatureVectorPair();

    void
    stepAndGenerate();
  
};


GenerateMergedVector::GenerateMergedVector(const SqlDatabase::TransactionPtr &tmp_tx, int _stride, int _windowSize, int _functionId,  
		std::vector<int> _variantNumVec)
: tx(tmp_tx), stride(_stride), windowSize(_windowSize), functionId(_functionId)
{
	currentVectorId = 0;
	currentStride = stride;

	for(unsigned int i = 0; i < windowSize; i++){
		mergedVectorArray.push_back( std::pair<SignatureVector*, SgLocatedNode*>(new SignatureVector(),NULL) );
	}
	for(unsigned int i = 0; i < _variantNumVec.size(); i++){
		variantNumVec.push_back(_variantNumVec[i]);
	}

};

std::pair<SignatureVector*, SgLocatedNode*>&
GenerateMergedVector::getSignatureVectorPair(){
	return  mergedVectorArray[currentVectorId];
};



void
GenerateMergedVector::stepAndGenerate()
{

	if(currentStride < stride){
		//Only happens when the stride is larger than the windowSize
		//ignoring this vector
		// - need to clear items that are set
		getSignatureVectorPair().first->clear();
		currentStride++;

	}else if(currentVectorId == windowSize - 1 ){

		//iterate over vecs and add up
		//PS! LIMITED TO ONLY WORK FOR SOURCE CODE CLONE DETECTION SINCE
		//V_SgNumVariants is the limit

		SgLocatedNode* firstNode = NULL;
		SgLocatedNode* lastNode  = NULL;

		for( unsigned int i = 0; i < mergedVectorArray.size(); i++ ){
			SignatureVector* tmp_vec = mergedVectorArray[i].first;

			if( firstNode == NULL ) 
				firstNode =  mergedVectorArray[i].second;

			if( mergedVectorArray[i].second  == NULL ) 
				lastNode  =  mergedVectorArray[i].second;

			for( unsigned int j = 0; j < V_SgNumVariants; j++ ){
				mergedVector.totalForVariant(j) += (*tmp_vec)[j]; 
			}  

		}
		addSourceVectorToDatabase(tx, mergedVector, functionId, firstNode, lastNode);         

		//jump for stride

		for(unsigned int i = stride; i < mergedVectorArray.size(); i++ ){
			std::pair<SignatureVector*,SgLocatedNode*> tmp_vec = mergedVectorArray[i - stride];
			mergedVectorArray[i - stride] = mergedVectorArray[i];
			mergedVectorArray[i] = tmp_vec;
		}

		currentVectorId = mergedVectorArray.size() >= stride ? mergedVectorArray.size() - stride : 0;   
		currentStride   = mergedVectorArray.size() >= stride ? stride : stride - mergedVectorArray.size(); 

		//Cleanup
		// - clear all elements of mergedVectorArray that will be overwritten
		// - clear mergedVector since vectors are already generated
		for(unsigned int i = currentVectorId; i < mergedVectorArray.size(); i++){ 
			mergedVectorArray[i].first->clear();
			mergedVectorArray[i].second = NULL;
		} 
		mergedVector.clear();



	}else{
		currentVectorId++;
                std::cerr << currentVectorId << std::endl;

	}

};


class CreateCloneDetectionVectors : public SgBottomUpProcessing<SynthesizedAttribute>
   {

     private:
       GenerateMergedVector* vecGen;

     public:

       CreateCloneDetectionVectors(const SqlDatabase::TransactionPtr &tmp_tx, int _functionId, size_t _minTokens, 
          size_t _stride, size_t _windowSize, std::vector<int> _variantNumVec, std::vector<int> _variantToWriteToFile) 
          : tx(tmp_tx), functionId(_functionId), minTokens(_minTokens), stride(_stride), windowSize(_windowSize),
            variantNumVec(_variantNumVec), variantToWriteToFile(_variantToWriteToFile)
       {
          firstNode = NULL;  
          lastNode  = NULL;
          vecGen = new GenerateMergedVector(tmp_tx, stride, windowSize, functionId,  
                       variantNumVec);
       };

       const SqlDatabase::TransactionPtr tx;
 
       std::map<SgNode*, int* > nodeToCloneVector; 

       int functionId;


       //min number of tokens before writing out
       int minTokens;
 
       //stride
       int stride;
       int windowSize;

       //Variants to put in the generated vector
       std::vector<int> variantNumVec;

       //Variants to write to file
       std::vector<int> variantToWriteToFile;



       SgLocatedNode* firstNode;

       SgLocatedNode* lastNode;


       // Functions required
       SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             SubTreeSynthesizedAttributes synthesizedAttributeList );

};




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

#if 1	 
	//Print out the number of elements found for each variant in the subtree
	for(int i = 0;  i < V_SgNumVariants; i++  ){
		if(returnAttribute.nodesInSubtree[i] > 0)
			std::cout << returnAttribute.nodesInSubtree[i] << " " << roseGlobalVariantNameList[i] << " ";
	}
	std::cout << "\n\n";
#endif

	//Write to the file specified in the config file on the commandline
	if(( variantToWriteToFile.size() == 0 || std::find(variantToWriteToFile.begin(), variantToWriteToFile.end(), astNode->variantT()) != variantToWriteToFile.end() ) && 
			astNode->get_file_info() != NULL && astNode->get_file_info()->isCompilerGenerated() == false 
	  )
	{

		int numTokens = 0;
               
		if( variantNumVec.size() > 0 ){
			for(unsigned int i=0; i < variantNumVec.size(); i++ )
				numTokens+= returnAttribute.nodesInSubtree[ variantNumVec[i] ];

		}else{
			for(unsigned int i=0; i < V_SgNumVariants; i++ )
				numTokens+= returnAttribute.nodesInSubtree[i];
		}


		if( numTokens >= minTokens )
		{
			//To implement the concept of a stride we need to not overcount
			//subtrees. But I am not sure if the vectors which contains this vector
			//should have a smaller
                        std::pair<SignatureVector*,SgLocatedNode*> mergedVectorPair = vecGen->getSignatureVectorPair();
                        SignatureVector* mergedVector = mergedVectorPair.first;
                        mergedVectorPair.second = isSgLocatedNode(astNode);

			if(variantNumVec.size() > 0){
				for(unsigned int i = 0;  i < variantNumVec.size(); i++  )
					mergedVector->totalForVariant(i)+= returnAttribute.nodesInSubtree[i];
			}else{
				for(unsigned int i = 0;  i < V_SgNumVariants; i++  )
					mergedVector->totalForVariant(i)+= returnAttribute.nodesInSubtree[i];
			}
 
                        vecGen->stepAndGenerate();
		}

	}

	return returnAttribute;
}







void createSourceVectorsForSubtree(SgNode* top, int functionId, size_t minTokens, size_t stride, size_t windowSize,
     std::vector<int> variantNumVec, std::vector<int> variantToWriteToFile,  const SqlDatabase::TransactionPtr &tx )
{
   CreateCloneDetectionVectors t(tx, functionId, minTokens, stride, windowSize, variantNumVec, variantToWriteToFile);
   
   t.traverse(top);     
   
};

void
createSourceVectorsRespectingFunctionBoundaries(SgNode* top, size_t minTokens, size_t stride, size_t windowSize,
                                          std::vector<int> variantNumVec, std::vector<int> variantToWriteToFile, const SqlDatabase::TransactionPtr &tx)
{
    CloneDetection::FilesTable files(tx);

    SqlDatabase::StatementPtr cmd1 = tx->statement("insert into semantic_functions"
                                                   // 0   1     2              3         4      5      6
                                                   " (id, file_id, name, entry_va, isize, dsize, size)"
                                                   " values(?,?,?,?,?,?,?)");
    
    vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition>(top);
    int functionId = tx->statement("select coalesce(max(id),-1)+1 from semantic_functions")->execute_int(); // zero origin
    
    for (vector<SgFunctionDefinition*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi, ++functionId) {

      size_t num_successors = (*fi)->get_numberOfTraversalSuccessors(); 
 
        std::string specimen_name = (*fi)->get_file_info()->get_filename();
        int specimen_id = files.insert(specimen_name);
        files.save(tx);

        cmd1->bind(0, functionId);
        cmd1->bind(1, specimen_id);
        cmd1->bind(2, (*fi)->get_mangled_name() );
        cmd1->bind(3, (*fi)->get_file_info()->get_line());
        cmd1->bind(4, num_successors );
        cmd1->bind(5, num_successors);
        cmd1->bind(6, num_successors);
        cmd1->execute();

	createSourceVectorsForSubtree(*fi, functionId, minTokens, stride, windowSize, variantNumVec, variantToWriteToFile, tx);
  
    }
    cerr << "Total vectors generated: " << numVectorsGenerated << endl;
}


