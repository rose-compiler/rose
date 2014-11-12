//Author: Andreas Saebjoernsen
//AS(091107) Tool to generate an input vector file for the
//Deccard clone detection tool. 

#include "rose.h"
#include "SqlDatabase.h"
#include "AST_FILE_IO.h"

#include "createSignatureVectors.h"

#include "../semantic/CloneDetectionLib.h"
#include "createCloneDetectionVectorsBinary.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>



using namespace std;



////////////////////////////////////////////////////////////////////
// BEGIN READING VARIANTS FROM FILE
////////////////////////////////////////////////////////////////////


//Given a vector of the names corresponding to variants,
//return an equivalent vector of the variant numbers
std::vector<int> 
fileToVariantT(std::vector<std::string> variantNames){
    std::vector<int> variantNumbers;
    for( unsigned int i =0; i < V_SgNumVariants; i++){
      for(unsigned int j = 0; j < variantNames.size(); j++){
         if(roseGlobalVariantNameList[i] == variantNames[j] )  
		 {
           variantNumbers.push_back(i);
		 }
	  }
    }
   
    return variantNumbers;
}

//Print all variants to file for inVector and toFile options
void
printGlobalVariantToFile(){
    std::ofstream myfile;
    myfile.open("all_variants", ios::app); 
    
    for( unsigned int i =0; i < V_SgNumVariants; i++){
         myfile << roseGlobalVariantNameList[i] << " "; 
    }
   
}

//Read all the words in a file into an vector of strings
std::vector<std::string> 
readFile( std::string filename){
     std::vector<std::string> variantsToUse;
     std::fstream file_op(filename.c_str());
     if (file_op.fail()) {
	  std::cout << "error: could not find file \"" << filename 
	           << "\" which is meant to include the styles to enforce with " 
		   << "the name checker." << std::endl;
			          exit(1);    // abort program
				      
				  }

     std::string current_word;

     while(file_op >> current_word){
       //First word denotes what the regular expression should operate
       //upon. Second word denotes the regular expression
       variantsToUse.push_back(current_word);
     }

    return variantsToUse;
}


//Option to specify the filename for the file which specifies which variants should 
//be in the generated vector
static const char* OPTION_NAMESTYLEFILE    = "*clone:inVector";

//Option to specify which files should have clone detection vectors outputted
//to file
static const char* OPTION_VARIANTSTOVECTOR = "*clone:toFile";

static const char* OPTION_MINTOKENS = "*clone:minTokens";

static const char* OPTION_STRIDE    = "*clone:stride";

static const char* OPTION_DATABASE    = "*clone:database";


//! Default command-line prefix for ROSE options
static const char* OPTION_PREFIX_ROSE = "-rose:";


//! Default command-option separator tag
static const char* OPTION_VALUE_SEPARATOR = "$^";


//! Wrapper around the SLA string option processing routine.
static	int
getRoseOptionValues (std::vector<std::string>& argv, const std::string& opt_name,
		std::vector<std::string>& values)
   {
     int num_matches = sla_str (argv,
		     OPTION_PREFIX_ROSE,
		     OPTION_VALUE_SEPARATOR,
		     opt_name,
		     (std::string*)NULL);
     if (num_matches > 0)
	{
	  std::vector<std::string> raw_values(num_matches);
	  sla_str (argv,
OPTION_PREFIX_ROSE, OPTION_VALUE_SEPARATOR, opt_name,
			  &raw_values[0]);
	  values.insert(values.end(), raw_values.begin(), raw_values.end());
	}
     return num_matches;
   }


#if 0 // JJW 10-17-2007 does not appear to be used
std::vector<int>
numberVecFromConfigFile(int* totalNumVec){
   

}
#endif

////////////////////////////////////////////////////////////////////
// END READING VARIANTS FROM FILE
////////////////////////////////////////////////////////////////////

using namespace std;



int main( int argc, char * argv[] ) {
	//printGlobalVariantToFile();
	std::vector<std::string> argvList(argv, argv + argc);


       std::map<SgNode*, int* > nodeToCloneVector; 

       //Variants to put in the generated vector
       std::vector<int> variantNumVec;

       //Variants to write to file
       std::vector<int> variantToWriteToFile;

       //stride
       int stride = 0;

       //min number of tokens before writing out
       int minTokens = 0;

	//Read in the variants which should be in the generated vector 
	std::vector<std::string> raw_conf_filename;
	getRoseOptionValues (argvList, OPTION_NAMESTYLEFILE, raw_conf_filename);


	if( raw_conf_filename.size() != 1  ){
		//std::cerr << "Usage: cloneDetection -rose:clone:inVector your_filename" << std::endl;
		//exit(1);
	}else{
		ROSE_ASSERT( raw_conf_filename.size() == 1);
		for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   ){
			std::string filename = raw_conf_filename[i];
			std::vector<std::string> variantNameVec = readFile(filename);           
			std::vector<int> variantNumVec = fileToVariantT(variantNameVec);  
			variantNumVec = variantNumVec;
		}

	}
	//Read in which variants should have a vector printed to file
	raw_conf_filename.clear();
	getRoseOptionValues (argvList, OPTION_VARIANTSTOVECTOR, raw_conf_filename);

	if( raw_conf_filename.size() != 1  ){
		//std::cerr << "Usage: cloneDetection -rose:clone:toFile your_filename" << std::endl;
		//exit(1);
	}else{
		ROSE_ASSERT( raw_conf_filename.size() == 1);
		for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   ){
			std::string filename = raw_conf_filename[i];
			std::vector<std::string> variantNameVec = readFile(filename);           
			std::vector<int> variantToWriteToFile = fileToVariantT(variantNameVec);
			variantToWriteToFile = variantToWriteToFile;
		}

	}


	//Find the minimum amount of tokens
	raw_conf_filename.clear();
	getRoseOptionValues (argvList, OPTION_MINTOKENS, raw_conf_filename);

	minTokens = 0;

	if( raw_conf_filename.size() != 1  ){
		std::cerr << "Usage: cloneDetection -rose:clone:minTokens $integer" << std::endl;
		exit(1);
	}else{
		ROSE_ASSERT( raw_conf_filename.size() == 1);
		for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   ){
			minTokens = atoi(raw_conf_filename[i].c_str());
		}

	}

	//Find the stride
	raw_conf_filename.clear();
	getRoseOptionValues (argvList, OPTION_STRIDE, raw_conf_filename);

	if( raw_conf_filename.size() != 1  ){
		std::cerr << "Usage: cloneDetection -rose:clone:stride $integer" << std::endl;
		exit(1);
	}else{
		ROSE_ASSERT( raw_conf_filename.size() == 1);
		for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   ){
			stride = atoi(raw_conf_filename[i].c_str());
		}

	}

        std::string database;
 	raw_conf_filename.clear();
	getRoseOptionValues (argvList, OPTION_DATABASE, raw_conf_filename);

	if( raw_conf_filename.size() != 1  ){
		std::cerr << "Usage: cloneDetection -rose:clone:database your_db_name" << std::endl;
		exit(1);
	}else{
		ROSE_ASSERT( raw_conf_filename.size() == 1);
		for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   ){
			database = raw_conf_filename[i];
		}

	}


	//for( int i=0; i < V_SgNumVariants; i++ )
	//	t.mergedVector.push_back(0);


        SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(database)->transaction();
 
        int windowSize = 10;
	// Save parameters in the database; or check against existing parameters
	if (0 == tx->statement("select count(*) from run_parameters")->execute_int()) {
		if ((size_t)-1 == stride || (size_t)-1 == windowSize) {
			std::cerr <<argv <<": stride and window size must be specified\n";
			exit(1);
		}
		tx->statement("insert into run_parameters (window_size, stride) values (?, ?)")
			->bind(0, windowSize)->bind(1, stride)->execute();
	} else {
		SqlDatabase::Statement::iterator params = tx->statement("select window_size, stride from run_parameters")->begin();
		size_t oldWindowSize = params.get<size_t>(0);
		size_t oldStride = params.get<size_t>(1);
		if ((size_t)-1==stride)
			stride = oldStride;
		if ((size_t)-1==windowSize)
			windowSize = oldWindowSize;
		if (oldWindowSize != windowSize || oldStride != stride) {
			std::cerr <<argv <<": window size (" <<windowSize <<") and stride (" <<stride <<") do not match"
				<<" existing window size (" <<oldWindowSize <<") and stride (" <<oldStride <<")\n";
			exit (1);
		}
	}



	SgProject* project = frontend(argvList);

        createSourceVectorsRespectingFunctionBoundaries(project, minTokens, stride, windowSize, variantNumVec, variantToWriteToFile, tx );
        
        tx->commit();

        return backend(project);

}

