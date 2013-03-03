

#include "MintPragmas.h"
#include "../mintTools/MintTools.h"

#include "rose.h"

using namespace std;
using namespace OmpSupport;
using namespace SageBuilder;
using namespace SageInterface;

void StringSplit(string str, string delim, vector<string>& results)
{

  size_t cutAt;
  while( (cutAt = str.find_first_of(delim)) != str.npos )
    {
      if(cutAt > 0)
	{
	  results.push_back(str.substr(0,cutAt));
	}
      str = str.substr(cutAt+1);
    }
  if(str.length() > 0)
      results.push_back(str);
}


string removeSpaces( string stringIn )
{
  string::size_type pos = 0;
  bool spacesLeft = true;

  while( spacesLeft )
    {
      pos = stringIn.find(" ");
      if( pos != string::npos )
	stringIn.erase( pos, 1 );
      else
	spacesLeft = false;
    }

  return stringIn;
} 


bool MintPragmas::isMintPragma(SgNode* node)
{
  //TODO: checks the mint pragma syntax 
  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  ROSE_ASSERT(decl);

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();

  //step1 : split the string with white spaces 
  //step2 : first one should be mint 
  //step3 : second one should be for/copy/parallel
  //step4 : if not return false : print unrecognized pragma 
  //step5 : 
  //if it is a copy pragma to device 
  //if it is a copy pragma to host 
  //if it is a clause to the for loop
  
  vector<string> constructs;

  //step1 : split the string  with white spaces 
  StringSplit(pragmaString, " ", constructs);
  
  if(constructs.size() >= 1 )
    {
      vector<string>::iterator it= constructs.begin(); 
      string tokenMint = (*it);      
      string tokenConstruct =""; 

      if( (++it) != constructs.end())      
	tokenConstruct = (*it);

      //#pragma mint bla bla ...
      if(tokenMint == MINT_PRAGMA )
	{
	  bool verbose= true;
	  if(tokenConstruct == MINT_FOR_PRAGMA ){
	    return isForLoopPragma(node, verbose);
	  }
	  else if (tokenConstruct == MINT_PARALLEL_PRAGMA){
	    return isParallelRegionPragma(node); 
	  }
	  else if (tokenConstruct == MINT_SINGLE_PRAGMA){
	    return isSinglePragma(node);
	  }
	  else if (tokenConstruct == MINT_COPY_PRAGMA){
	    if(isTransferToFromDevicePragma(node, verbose))
	      {
	      //#ifdef VERBOSE_2
	      cout << "  INFO:Mint: @ Line "  << decl->get_file_info()->get_line() 
		   << "  Recognized pragma mint copy " << endl << endl; 
	      //#endif
	      return true;
	    }
	  }
	  else if(tokenConstruct == "")
	    {
	      cout << "  WARNING:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
		   << "  Incomplete pragma, did you forget to add a directive? Get some sleep." << endl << endl;
	      //ROSE_ABORT();
	    }
	  /*
	  else {
	    cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
		 << "  Unrecognized construct --> "<< tokenConstruct  << endl;
	    ROSE_ABORT();
	    }*/

	} //end of Mint Pragma 
    }

  //cout << "  WARNING:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
  //     << "  Unrecognized/Incomplete pragma !!!" << endl;

  return false;
}

bool MintPragmas::isParallelRegionPragma(SgNode* node)
{

  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  if(!decl)
    return false; 

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();

  vector<string> constructs; 

  StringSplit(pragmaString, " ", constructs);

  vector<string>::iterator it= constructs.begin(); 
  string tokenMint = (*it);      
  string tokenConstruct =""; 
  
  if( (++it) != constructs.end())      
    tokenConstruct = (*it);
  else 
    return false; 
  //#pragma mint bla bla ...
  if(tokenMint != MINT_PRAGMA )
      return false;
  if(tokenConstruct != MINT_PARALLEL_PRAGMA )
    return false; 

  SgStatement* nextStmt = getNextStatement(decl);
  ROSE_ASSERT(nextStmt);

  if(isSgOmpParallelStatement(nextStmt))
    nextStmt = getNextStatement(nextStmt);

  if(!isSgBasicBlock(nextStmt))
    {
      cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	   << "  Ooops,a parallel region should be followed by a basic block." << endl;
      ROSE_ABORT();            
    }

  return true;

}


bool MintPragmas::isSinglePragma(SgNode* node)
{

  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);

  if(!decl)
    return false;

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();

  vector<string> constructs; 

  StringSplit(pragmaString, " ", constructs);

  vector<string>::iterator it= constructs.begin(); 
  string tokenMint = (*it);      
  string tokenConstruct =""; 
  
  if( (++it) != constructs.end())      
    tokenConstruct = (*it);
  else 
    return false; 
  //#pragma mint bla bla ...
  if(tokenMint != MINT_PRAGMA )
      return false;
  if(tokenConstruct != MINT_SINGLE_PRAGMA )
    return false; 

  SgStatement* nextStmt = getNextStatement(decl);
  
  if(!isSgBasicBlock(nextStmt))
    {
      cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	   << "  Ooops,a single region should be followed by a basic block." << endl;
      ROSE_ABORT();            
    }


  return true;

}



bool MintPragmas::isForLoopPragma(SgNode* node, bool verbose)
{
  //nested() threadblock() 
  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  ROSE_ASSERT(decl);

  /*
  //the statement should follow by an omp for statement, no it is not a requirement anymore
  if(!isSgOmpForStatement(nextStmt)){
    cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	 << "  Ooops, no matching omp for pragma after  mint pragma, forget to add #pragma omp for?"  << endl;
    ROSE_ABORT();
    }*/

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();

  vector<string> constructs; 

  StringSplit(pragmaString, " ", constructs);

  vector<string>::iterator it= constructs.begin(); 
  string tokenMint = (*it);      
  string tokenConstruct =""; 
  
  if( (++it) != constructs.end())      
    tokenConstruct = (*it);
  else 
    return false; 
  //#pragma mint bla bla ...
  if(tokenMint != MINT_PRAGMA )
      return false;
  if(tokenConstruct != MINT_FOR_PRAGMA )
    return false; 

  if(!verbose)
    return true;

  SgStatement* nextStmt = getNextStatement(decl);
  ROSE_ASSERT(nextStmt);

  if(isSgOmpForStatement(nextStmt))
    nextStmt = isSgOmpForStatement(nextStmt) -> get_body();

  ROSE_ASSERT(nextStmt);

  //next statement should be a forloop or omp for
  if(!isSgForStatement(nextStmt))
    {
      cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	   << "  Ooops, no matching for loop after the mint for. Did you misplace the directive? Hungry?"  << endl;
      ROSE_ABORT();      
    }

  size_t nestingLevel = 1;
  string nested = "1";

  //processing nested clauses 
  //nested(...) clause 

  size_t pos_nested = pragmaString.find(MINT_NESTED_PRAGMA) ;
  if(pos_nested != string::npos)
    {
      nested= pragmaString.substr(0, pragmaString.find(MINT_NESTED_PRAGMA) + 
					 MINT_NESTED_PRAGMA.length());
  
      //nested = pragmaString.substr (nested.length() + 1);
      // Liao 2/6/2013, we need to preserve the leading '('
      nested = pragmaString.substr (nested.length());
      
      //get rid off the paranthesis around the parameter list 
      nested = nested.substr(nested.find("(")+1 , nested.find_first_of(")")-1);
      
      //clean the string from spaces 
      nested = removeSpaces(nested);
      
      if(nested != MINT_ALL_PRAGMA && atoi(nested.c_str()) == 0 )
	{//only numerics and all  are allowed
	  cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	       << "  Ooops, nested loop level is not a number! can be either [1-9] or all!" << endl; 
	  ROSE_ABORT() ;      
	}
      if(nested != MINT_ALL_PRAGMA)
	{
	  nestingLevel = atoi(nested.c_str());
	  
	  std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(nextStmt,V_SgForStatement);
	  
	  //check if the number for loops are >= nested	 
	  if(nestingLevel > loopNest.size()){
	    cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
		 << "  Ooops, nested loop level exceeds the actual number of loops!" << endl; 
	    ROSE_ABORT() ; 
	  }
	}
      else
	{
	  std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(nextStmt,V_SgForStatement);
	  nestingLevel = loopNest.size();
	}
    }
  //threadblock(16, 64, 1) x,y,z
  //processing threadblock()
  size_t pos_th = pragmaString.find(MINT_TILE_PRAGMA) ;

  size_t total = 1; 

  if( pos_th != string::npos )
    {
      string threadblock= pragmaString.substr(0, pragmaString.find(MINT_TILE_PRAGMA) + 
					      MINT_TILE_PRAGMA.length());
      //threadblock = pragmaString.substr (threadblock.length() + 1);
      threadblock = pragmaString.substr (threadblock.length());
      
      //get rid off the paranthesis around the parameter list 
      threadblock = threadblock.substr(threadblock.find("(")+1 , threadblock.find_first_of(")")-1);
      
      //clean the string from spaces 
      threadblock = removeSpaces(threadblock);
      
      vector<string> threadsizes;
      
      StringSplit(threadblock, ",", threadsizes );
      
      for(vector<string>::iterator it = threadsizes.begin() ; it!= threadsizes.end(); it++)
	{
	  string bsize = (*it);
	  
	  if(atoi(bsize.c_str()) == 0 )
	    {
	      cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
		   << "  Ooops, threadblock accepts only numerics for now :(" << endl; 
	      ROSE_ABORT() ;      
	    }
	  
	  total = total * atoi(bsize.c_str());
	}
      
      if(threadsizes.size() > nestingLevel && nested != MINT_ALL_PRAGMA)
	{
	  cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	       << "  Number of thread block param should be <= nesting level " << endl; 
	  ROSE_ABORT() ;      
	}
    }

  //chunksize(1, 2, 16) x,y,z
  //processing unroll
  size_t pos = pragmaString.find(MINT_CHUNKSIZE_PRAGMA) ;
  size_t totalUnroll = 1;

  if (pos !=string::npos)
   {
     string unrollfactor= pragmaString.substr(0, pragmaString.find(MINT_CHUNKSIZE_PRAGMA) +
					      MINT_CHUNKSIZE_PRAGMA.length());
     
     //unrollfactor = pragmaString.substr (unrollfactor.length() + 1);
     unrollfactor = pragmaString.substr (unrollfactor.length());

     //get rid off the paranthesis around the parameter list 
     unrollfactor = unrollfactor.substr(unrollfactor.find("(")+1 , unrollfactor.find_first_of(")")-1);
     
     //clean the string from spaces 
     unrollfactor = removeSpaces(unrollfactor);
     
     vector<string> unrollsizes;
     
     StringSplit(unrollfactor, ",", unrollsizes );
     
     for(vector<string>::iterator it = unrollsizes.begin() ; it!= unrollsizes.end(); it++)
       {
	 string bsize = (*it);
	 
	 if(atoi(bsize.c_str()) == 0 )
	   {
	     cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
		  << "  Ooops, chunksize value can be only numerics " << " Exiting ..."<< endl; 
	     ROSE_ABORT() ;      
	   }
	 totalUnroll = totalUnroll * atoi(bsize.c_str());

       }

     if( (unrollsizes.size() > nestingLevel) || unrollsizes.size() > 3)
       {
	 cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << " Exiting ..."<< endl 
	      << "  Too many chunksize params " << endl; 
	 ROSE_ABORT() ;      
       }
   }

  if(total/(totalUnroll) > (size_t)MAX_NUM_THREADS_PER_BLOCK)
    {
      cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	   << "  Requested too many threads/block, max can be "<< MAX_NUM_THREADS_PER_BLOCK << " Exiting ..."<< endl; 
      ROSE_ABORT() ;            
    }

  //#ifdef VERBOSE
  if(verbose){
    cout << "  INFO:Mint: @ Line "  << decl->get_file_info()->get_line() << 
	    "  Recognized pragma mint for " << endl << endl; 
  }
  //#endif
  //there might be inconsistent chunksizes
  //need to check that. 

  return true; 
}

void doSomeChecking(SgNode* node, MintForClauses_t params)
{
  //nested() threadblock() 
  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  ROSE_ASSERT(decl);

  bool chunk_err = false;

  //check if the chunksize is larger than the tile size or tile is divisible by chunksize 
  if(params.tileDim.x / params.chunksize.x == 0 || params.tileDim.x % params.chunksize.x != 0)
    chunk_err = true;
  if(params.tileDim.y / params.chunksize.y == 0 || params.tileDim.y % params.chunksize.y != 0)
    chunk_err = true;
  if(params.tileDim.z / params.chunksize.x == 0 || params.tileDim.z % params.chunksize.z != 0)
    chunk_err = true;

  if(chunk_err){
      cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	   << "  Tile size should be divisible by chunksize !!! Exiting ..."<< endl;
      ROSE_ABORT() ;                  
  }

  //chunking in x dimension is not recommended. 
  if(params.chunksize.x !=  1 ){
      cout << "  WARNING:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
	   << "  Setting chunksize.x to 1 is HIGHLY RECOMMENDED. Make sure you really want to set it to " << params.chunksize.x << endl << endl;
  }
  
}

MintForClauses_t MintPragmas::getForParameters(SgNode* node)
{
  MintForClauses_t params;

  //set default values 
  params.nested = 1 ; //only outmost loop is parallelizable 
  params.tileDim.x = DEFAULT_BLOCKDIM;
  params.tileDim.y = 1 ;
  params.tileDim.z = 1;
  params.chunksize.x = 1;
  params.chunksize.y = 1;
  params.chunksize.z = 1;

  //nested() threadblock() unroll()
  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  if(decl == NULL)
    return params;

  ROSE_ASSERT(decl);

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();


  vector<string> constructs; 
  StringSplit(pragmaString, " ", constructs);

  vector<string>::iterator it= constructs.begin(); 
  string tokenMint = (*it);      
  string tokenConstruct =""; 
  
  if( (++it) != constructs.end())      
    tokenConstruct = (*it);
  else 
    return params; 
  //#pragma mint bla bla ...
  if(tokenMint != MINT_PRAGMA )
      return params;
  if(tokenConstruct != MINT_FOR_PRAGMA )
    return params; 

  string nested = "1";
      
  //processing nested clauses 
  //nested(...) clause     
  size_t pos_nested = pragmaString.find(MINT_NESTED_PRAGMA) ;

  if(pos_nested != string::npos)
    {
      nested = pragmaString.substr(0, pragmaString.find(MINT_NESTED_PRAGMA) + 
				  MINT_NESTED_PRAGMA.length());
	  
      //nested = pragmaString.substr (nested.length() + 1);
      nested = pragmaString.substr (nested.length());
	  
      //get rid off the paranthesis around the parameter list 
      nested = nested.substr(nested.find("(")+1 , nested.find_first_of(")")-1);
      
      //clean the string from spaces 
      nested = removeSpaces(nested);

      SgStatement* nextStmt = getNextStatement(decl);
      ROSE_ASSERT(nextStmt );

      if(nested == MINT_ALL_PRAGMA){
	std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(nextStmt,V_SgForStatement);
	params.nested = loopNest.size(); 
      }
      else 
	{
	  params.nested = atoi(nested.c_str());
	}
    }

  //reset the default values for block sizes 
  if(params.nested >= 2)
    params.tileDim.y = DEF_BLOCKDIM_Y ;
  if (params.nested == 3)
    params.tileDim.z = DEF_BLOCKDIM_Z ;
 
 //threadblock(16, 64, 1) x,y,z
  //processing threadblock()
  size_t pos_th = pragmaString.find(MINT_TILE_PRAGMA) ;

  if( pos_th != string::npos )
    {
      string threadblock= pragmaString.substr(0, pragmaString.find(MINT_TILE_PRAGMA) + 
					      MINT_TILE_PRAGMA.length());
      //threadblock = pragmaString.substr (threadblock.length() + 1);
      threadblock = pragmaString.substr (threadblock.length());
      
      //get rid off the paranthesis around the parameter list 
      threadblock = threadblock.substr(threadblock.find("(")+1 , threadblock.find_first_of(")")-1);
      
      //clean the string from spaces 
      threadblock = removeSpaces(threadblock);
      
      vector<string> blocksizes;
      
      StringSplit(threadblock, ",", blocksizes );
      
      if(blocksizes.size() >= 1)
	params.tileDim.x = atoi(blocksizes[0].c_str());
      if(blocksizes.size() >= 2)
	  params.tileDim.y = atoi(blocksizes[1].c_str());	
      if(blocksizes.size() >= 3)
	  params.tileDim.z = atoi(blocksizes[2].c_str());
    } //end of if 

  //unroll(1, 2, 16) x,y,z
  //processing unroll
  size_t pos = pragmaString.find(MINT_CHUNKSIZE_PRAGMA) ;
  if (pos !=string::npos)
   {
     string unrollfactor= pragmaString.substr(0, pragmaString.find(MINT_CHUNKSIZE_PRAGMA) +
					      MINT_CHUNKSIZE_PRAGMA.length());     
     //unrollfactor = pragmaString.substr (unrollfactor.length() + 1);
     unrollfactor = pragmaString.substr (unrollfactor.length());

     //get rid off the paranthesis around the parameter list 
     unrollfactor = unrollfactor.substr(unrollfactor.find("(")+1 , unrollfactor.find_first_of(")")-1);
     
     //clean the string from spaces 
     unrollfactor = removeSpaces(unrollfactor);
     
     vector<string> unrollsizes;
     
     StringSplit(unrollfactor, ",", unrollsizes );

     if(unrollsizes.size() >= 1)
       params.chunksize.x = atoi(unrollsizes[0].c_str());
     if(unrollsizes.size() >= 2)
       params.chunksize.y = atoi(unrollsizes[1].c_str());	
     if(unrollsizes.size() >= 3)
       params.chunksize.z = atoi(unrollsizes[2].c_str());
     
   }

  doSomeChecking(node, params);
  
  //there might be inconsistent unroll sizes 
  //need to check that. 
  return params; 
}



MintTrfParams_t MintPragmas::getTransferParameters(SgNode* node, 
						   const MintHostSymToDevInitMap_t hostToDevVars)
{
  MintTrfParams_t params;

//#pragma mint copy(Unew, toDevice, (n+2), (m+2), (k+2))

  ROSE_ASSERT(node != NULL);

  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  ROSE_ASSERT (decl != NULL);
  
  SgScopeStatement* scope = decl->get_scope();
  ROSE_ASSERT(scope);

  SgStatement* stmt = isSgStatement(decl);
  ROSE_ASSERT(stmt);

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();

  //processing copy clauses 
  string mintCopy= pragmaString.substr(0, pragmaString.find(MINT_COPY_PRAGMA) + 
				       MINT_COPY_PRAGMA.length());
  
  //mintCopy = pragmaString.substr (mintCopy.length() + 1);
  mintCopy = pragmaString.substr (mintCopy.length());
  
  //get rid off the paranthesis around the parameter list 
  mintCopy = mintCopy.substr(mintCopy.find("(")+1 , mintCopy.find_last_of(")")-1);
  
  //clean the string from spaces 
  mintCopy = removeSpaces(mintCopy);
  
  vector<string> copyParams;

  StringSplit(mintCopy, ",", copyParams);

  vector<string>::iterator it= copyParams.begin();
 
  params.dst = (*it++);
  //we removed the device pointer from the params
  //params.src = (*(it++)); 
  params.trfType = (*(it++));

  //Added in 15 March 2011
  //We removed the device variable from the params
  //the compiler will give a name to the device pointer
  //We need to generate a device variable but we cannot do that for the 
  //dev to host transfer, there should be a name used before for that variable
  if(params.trfType == MINT_DATA_TRF_TO_DEV_PRAGMA)
    {
      params.src = params.dst ;
      params.dst = MintTools::generateDeviceVarName(stmt) + params.src ; 
    }
  else {

    SgFunctionDeclaration* func= SageInterface::getEnclosingFunctionDeclaration(scope);

    SgInitializedName* src = MintTools::findInitName(isSgNode(func), params.dst );
    ROSE_ASSERT(src);

    SgVariableSymbol* host_sym = isSgVariableSymbol(src->get_symbol_from_symbol_table ());   
    ROSE_ASSERT(host_sym);

    if(hostToDevVars.find(host_sym) == hostToDevVars.end())
      {
	//we will create the variable on the memory manager
	params.src = MintTools::generateDeviceVarName(stmt) + params.dst ; 

	//cout << "  ERROR:Mint: Couldn't find host symbol in " << pragmaString << endl;
	//ROSE_ABORT();
      }
    else 
      {
	MintHostSymToDevInitMap_t::const_iterator it = hostToDevVars.find(host_sym);
	
	SgInitializedName* dev_name = it->second;
	ROSE_ASSERT(dev_name);
	
	params.src = dev_name -> get_name().str();
      }
  }
  for(; it != copyParams.end(); it++){
    string arr_size = (*it);
    arr_size = "(" + arr_size + ")";
    (params.sizes).push_back(arr_size);
  }

  //if dim < 3, set the sizes in y and z to 1 
  if(params.sizes.size()==1)
    (params.sizes).push_back("1");
  
  if(params.sizes.size()==2)
    (params.sizes).push_back("1");
  
  return params;
}


bool MintPragmas::isTransferToFromDevicePragma(SgNode* node, bool verbose)
{
//#pragma mint copy(Uold, toDevice, (n+2), (m+2), (k+2))
//#pragma mint copy(Unew, toDevice, (n+2), (m+2), (k+2))
  ROSE_ASSERT(node != NULL);

  SgPragmaDeclaration* decl = isSgPragmaDeclaration(node);
  if(decl == NULL)
    return false;

  SgScopeStatement* scope = decl->get_scope();

  SgPragma* pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma ();

  vector<string> constructs;
  StringSplit(pragmaString, " ", constructs);

  vector<string>::iterator it= constructs.begin(); 
  string tokenMint = (*it);      
  string tokenConstruct =""; 
  
  if( (++it) != constructs.end())      
    tokenConstruct = (*it);
  else 
    return false; 

  //#pragma mint bla bla ...
  if(tokenMint != MINT_PRAGMA )
      return false;
  if(tokenConstruct != MINT_COPY_PRAGMA )
    return false; 

  if(verbose)
    {
      //processing copy clauses 
      string mintCopy= pragmaString.substr(0, pragmaString.find(MINT_COPY_PRAGMA) + 
					   MINT_COPY_PRAGMA.length());

      // Liao 2/6/2013. We have to preserve the leading '('
      //mintCopy = pragmaString.substr (mintCopy.length() + 1);
      mintCopy = pragmaString.substr (mintCopy.length());
      
      //get rid off the paranthesis around the parameter list 
      mintCopy = mintCopy.substr(mintCopy.find("(")+1 , mintCopy.find_last_of(")")-1);
      
      //clean the string from spaces 
      mintCopy = removeSpaces(mintCopy);
      
      vector<string> copyParams;
      string trftype; 
      
      StringSplit(mintCopy, ",", copyParams);
      
      if(copyParams.size() < 3 || copyParams.size() > 5 )
	{
	  cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl;
	  if(copyParams.size()< 3){ 
	    cout   << "  Too few  arguments in mint copy parameters :(" << endl; 
	    cout   << "  e.g copy(src|dst, toDevice|fromDevice, size-x,size-y,size-z)" << endl; 
	  }
	  else {
	    cout   << "  Too many  arguments in mint copy parameters :(" << endl; 
	    cout   << "  e.g copy(src|dst, toDevice|fromDevice, size-x,size-y,size-z)" << endl; 
	  }
	  ROSE_ABORT() ;      
	}
      else{
	//toDevice or fromDevice
	trftype = copyParams[1] ;
	if(trftype != MINT_DATA_TRF_TO_DEV_PRAGMA && trftype != MINT_DATA_TRF_FROM_DEV_PRAGMA)
	  {
	    cout << "  ERROR:Mint: @ Line "  << decl->get_file_info()->get_line() << endl 
		 << "  Unrecognized transfer type --> " << trftype << endl 
		 << "  Should be either " << MINT_DATA_TRF_TO_DEV_PRAGMA << " or " << MINT_DATA_TRF_FROM_DEV_PRAGMA << endl; 
	    ROSE_ABORT() ;      	
	  }
      }
      
      //added Oct 16, 2011
      //check if the pragma appears right before a parallel region
      //or in a parallel region 
      if(trftype == MINT_DATA_TRF_TO_DEV_PRAGMA)
	{
	  SgStatement* next = getNextStatement(isSgStatement(node));

	  //this statement either should be a parallel statement or another copy statement
	  if(isTransferToFromDevicePragma(isSgNode(next)))
	    return true;//this doesn't check if it is toDev or fromDev 
	  
	  if (isSgOmpParallelStatement(next))
	    return true;
	  
	  if (MintTools::getEnclosingParallelRegion(scope)) //check if it is inside of a parallel region 
	    return true;
	  
	  cout << "  WARNING:Mint: @ Line "  << decl->get_file_info()->get_line() 
	       << "  Ignoring copy directive " << endl
	       << "  toDevice should appear right before a parallel region or inside of a parallel region"<<endl<< endl;
	  
	  //print a warning here 
	  return false;
	}
      else 
	{
	  SgStatement* prev = getPreviousStatement(isSgStatement(node));
	  //this statement either should be a parallel statement or another copy statement
	  if(isTransferToFromDevicePragma(isSgNode(prev)))
	    return true;//this doesn't check if it is toDev or fromDev 
	  else if (isSgOmpParallelStatement(prev))
	    return true;
	  else if (MintTools::getEnclosingParallelRegion(scope)) //check if it is inside of a parallel region 
	    return true; 
	  
	  cout << "  WARNING:Mint: @ Line "  << decl->get_file_info()->get_line() 
	       << "  Ignoring copy directive" << endl
	       << "  fromDevice should appear right after a parallel region or inside of a parallel region"<< endl<<endl;
	  //print a warning here 
	  return false;
	}
    }
  return true; 
}



void MintPragmas::processForLoopMintPragmas(SgOmpForStatement* omp_for, 
					    MintForClauses_t& clauses)
{
  //MintForClauses 
  //clauses.nested = 0 means all 
  //clauses.nested = 1,2,3 are legal values, default is 0
  ROSE_ASSERT(omp_for);

  //mint loop pragmas should immediately appear before the omp for pragma
  SgStatement* prev = getPreviousStatement(omp_for);
  SgPragmaDeclaration* decl = isSgPragmaDeclaration(prev);

  //by this time, we should have checked the syntax already. 
  if(decl == NULL){
    cout << "  INFO:Mint: @ Line "  << prev->get_file_info()->get_line() << endl ;
    cout << "  No mint for loop pragma specified before the omp for." << endl ;
    cout << "  Using default valuues: nest(1), threads(128), (no unrolling) " << endl << endl;
      clauses.nested = 1 ;//only outher loop is parallelized 
      clauses.tileDim.x = 128 ;
      clauses.tileDim.y =  1;
      clauses.tileDim.x =  1;
    }
  else
    {
      //example: mint for nested() threadblock() unroll()
      //clauses.nested = getNestingLevel();

      SgPragma* pragma = decl->get_pragma();
      string pragmaString = pragma->get_pragma ();

      //processing nested clauses 
      //nested(...) clause 
      string nested= pragmaString.substr(0, pragmaString.find(MINT_NESTED_PRAGMA) + 
					 MINT_NESTED_PRAGMA.length());
 
      //nested = pragmaString.substr (nested.length() + 1);
      nested = pragmaString.substr (nested.length());
  
      //get rid off the paranthesis around the parameter list 
      nested = nested.substr(nested.find("(")+1 , nested.find_first_of(")")-1);
  
      //clean the string from spaces 
      nested = removeSpaces(nested);
      
      if(nested == MINT_ALL_PRAGMA)
	clauses.nested = 0; //means all the loops in the body are parallelizable
      else //if (nested == "1" || nested == "2" || nested == "3")
	{  

	  clauses.nested = atoi(nested.c_str()); 
	  //if it is not a valid number then it will abort 
	  if(clauses.nested == 0){
	    cout << "  ERROR:Mint: @ Line "  << prev->get_file_info()->get_line() << endl 
                 << "  Ooops, nested loop level is not a number! can be either [1-9] or all!" << endl; 
	    ROSE_ABORT() ;
	  }

	  std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(omp_for,V_SgForStatement);

	  //check if the number for loops are >= nested	 
	  if(clauses.nested > (int)loopNest.size()){
	    cout << "  ERROR:Mint: @ Line "  << prev->get_file_info()->get_line() << endl 
                 << "  Ooops, nested loop level exceeds the actual number of loops!" << endl; 
	    ROSE_ABORT() ; 
	  }
	}
    }
}

