/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
//#include "rose.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdarg.h> 
#include <assert.h>

#include "RuntimeSystem.h"


using namespace std;


RuntimeSystem* RuntimeSystem::pinstance = 0;// initialize pointer
RuntimeSystem* RuntimeSystem::Instance () {
  if (pinstance == 0)  // is it the first call?
    {  
      pinstance = new RuntimeSystem(); // create sole instance
    }
  return pinstance; // address of sole instance
}


RuntimeSystem::RuntimeSystem() 
{ 
  //... perform necessary instance initializations 
  std::cerr << " RtedConstructor . "  << std::endl;

  arrays1D.clear();
  myfile = new std::ofstream("result-array.txt",std::ios::app);
  if (!myfile) {
    std::cerr << "Cannot open output file." << std::endl;
    exit(1);
  }
  violation=false;
  violationNr=0;
  filename="";
  oldFilename="";
  fileNr=0;
  arrayDebug=false;
}



// ***************************************** HELPER FUNCTIONS *************************************
std::string 
RuntimeSystem::resBool(bool val) {                                               
  if (val)                                                                      
    return "true";                                                           
  return "false";                                                          
}


char*
RuntimeSystem::findLastUnderscore(char* s) {
  string ss = roseConvertToString(s);
  string name = "";
  int pos = ss.rfind("_");
  if (pos!=(int)std::string::npos) {
    name = ss.substr(pos,ss.length());
  }
  return (char*) name.c_str();
}

void
RuntimeSystem::callExit(char* filename, char* line, char* reason) {
  // violation Found ... dont execute it - exit normally
  cerr << "Violation found: " << reason << "    in file : " << filename << " at line: " << line << endl;
  exit(0);
}


template<typename T> std::string 
RuntimeSystem::roseConvertToString(T t) {
  std::ostringstream myStream; //creates an ostringstream object
  myStream << t << std::flush;
  return myStream.str(); //returns the string form of the stringstream object
};

char* 
RuntimeSystem::roseConvertIntToString(int t) {
  std::string conv = roseConvertToString(t);
  int size = conv.size();
  char* text = (char*)malloc(size);
  if (text)
    strcpy(text,conv.c_str());
  std::cerr << "String converted from int : " <<  text << std::endl;
  return text;
}


void 
RuntimeSystem::roseRtedClose() {
  if (myfile)
    myfile->close();
  std::cerr << " RtedClose :: Violation : " << resBool(violation) << std::endl;
  if (violationNr>3) {
    std::cerr << "RtedClose:: Nr of violations : " << violationNr << " is suspicious. " << std::endl;
    exit(1);
  }
  if (violation==false)  {
    std::cerr << "RtedClose:: No violation found!! " << filename << std::endl;
    exit(1);
  } else
    std::cerr <<"RtedClose:: Violation found. Good! " << filename << std::endl;
}

// ***************************************** HELPER FUNCTIONS *************************************








// ***************************************** ARRAY FUNCTIONS *************************************

char*
RuntimeSystem::findVariablesOnStack(char* name) {
  char* mang_name = NULL;
  std::vector<RuntimeVariables*>::const_reverse_iterator stack = 
    runtimeVariablesOnStack.rbegin();
  for (;stack!=runtimeVariablesOnStack.rend();++stack) {
    RuntimeVariables* rv = *stack;
    if (strcmp(name,rv->name)==0) {
      mang_name=rv->mangled_name;
    }
  }
  return mang_name;
}



/* -----------------------------------------------------------
 * create array and store its size
 * -----------------------------------------------------------*/
void
RuntimeSystem::roseCreateArray(char* name, int dimension, bool stack, long int sizeA, long int sizeB, 
			       char* filename, char* line ){
  if (arrayDebug)
    cout << fileNr << ": >>> Called : roseCreateArray : " << findLastUnderscore(name) << " dim"<< dimension <<
      " - [" << sizeA << "][" << sizeB << "] file : " << filename << " line : " << line  << endl;
  oldFilename=filename;
  if (dimension==1) {
    arrays1D[name]=sizeA;
    if (arrayDebug)
      cerr << ".. Creating 1Dim array - size : " << sizeA << endl;
  }
  else if (dimension==2) {
    // check if exist
    std::map<std::string, Array2D*>::const_iterator it = arrays2D.find(name);
    Array2D* array =NULL;
    if (it!=arrays2D.end()) {
      // array exists
      array = it->second;
      long int totalsize = array->size1;
      if (arrayDebug)
	cerr << "..    Expanding 2nd-run 2Dim array - sizeA : " << sizeA << "  sizeB : " << sizeB << endl;
      if (sizeA<0 || sizeA>=totalsize) {
	  cerr << " Violation detected :  Array too small to allocate more memory " << endl;
	violation=true;
	violationNr++;
	// this is a weird error, lets stop here for now.
	exit(1);
      } else {
	if (arrayDebug)
	  cerr << " >>> CREATING Array : arr ["<<totalsize<<"]["<<sizeB<<"]"<< "  alloc : ["<<sizeA<<"]="<<sizeB<<endl;
	array->allocate(sizeA,sizeB); //arr[sizeA][sizeB]
      }
    } else {
      // new array
      array = new Array2D(sizeA);// ptr [][] = malloc (20)   20 == totalsize
      arrays2D[name]=array;
      if (arrayDebug)
	cerr << ".. Creating 2Dim array - size : " << sizeA << endl;
      if (sizeB!=-1) {
	// expand this stack array
	for (int i=0;i<sizeA;++i)
	  array->allocate(i,sizeB); //arr[i][sizeB]
      if (arrayDebug)
	cerr << "..    Expanding 2Dim array - sizeA : " << sizeA << "  sizeB : " << sizeB << endl;
      }
    }
  }
}

/* -----------------------------------------------------------
 * check if array is out of bounds
 * -----------------------------------------------------------*/
void
RuntimeSystem::roseArrayAccess(char* name, int posA, int posB, char* filename, char* line){
  filename=filename;
  if (arrayDebug) 
    cout << "    Called : roseArrayAccess : " << findLastUnderscore(name) << " ... ";

  // check the stack if the variable is part of a function call
  char* mangl_name=findVariablesOnStack(name);  
  if (mangl_name)
    name=mangl_name;

  map<string,int>::const_iterator it = arrays1D.find(name);
  if (it!=arrays1D.end()) {
    int size = it->second;
    cout << "       Found 1Dim array : " << "  size: " << size << ",   access: [" << posA <<"]"<< endl;
    if (posB!=-1) {
      cerr << " Seems like this is not a valid 1Dim array : " << filename << "  line : " << line << endl;
      exit(1);
    }
    if (posA>=size || posA<0) {
      if (arrayDebug) {
	cout << "  >>>>>> Violation detected : Array size: " << size << " accessing: " << posA <<
	  "  in : " << filename << "  line: " << line << endl;
	*myfile << "  >>>>>> Violation detected : Array size: " << size << " accessing: " << posA <<
	  "  in : " << filename << "  line: " << line << endl;
      }
      violation=true;
      violationNr++;
      callExit(filename, line, (char*)"1Dim: Accessing array out of bounds");
    }
  }

  //  cerr << " Done with 1Dim array" << endl;
  std::map<std::string, Array2D*>::const_iterator it2 = arrays2D.find(name);
  if (it2!=arrays2D.end()) {
    Array2D* array = it2->second;
    int sizeA = array->size1;
    int sizeB = array->getSize(posA);
    if (arrayDebug) 
      cout << "  Found 2Dim array :  size: [" << sizeA << "][" << sizeB << "]  pos: [" << posA << "][" << posB << "]" <<endl;
    // allow arr[posA][posB] && arr[posA]  both 2Dim!
    if ((posA>=sizeA || posA<0) || posB>=sizeB || posB<0) {
      if (arrayDebug) {
	cout << "  >>>>>> Violation detected : Array size: [" << sizeA << "]["<<sizeB<<"] accessing: ["  << posA <<
	  "][" << posB << "]  in : " << filename << "  line: " << line << endl;
	*myfile << "  >>>>>> Violation detected : Array size: [" << sizeA << "]["<<sizeB<<"] accessing: ["  << posA <<
	  "][" << posB << "]  in : " << filename << "  line: " << line << endl;
      }
      violation=true;
      violationNr++;
      callExit(filename, line, (char*)"2Dim: Accessing array out of bounds");
    }
  } 
  //  cerr << " Done with 2Dim array" << endl;
  if (it==arrays1D.end() && it2==arrays2D.end()) {
    std::cout << endl;
    std::cerr << " >>> No such array was created. Can't access it. " << filename << "  l: " << line << endl;
    exit(1);
  }

}

// ***************************************** ARRAY FUNCTIONS *************************************








// ***************************************** FUNCTION CALL *************************************

void 
RuntimeSystem::handleNormalFunctionCalls(std::vector<char*>& args, char* filename, char* line) {
  cerr << "Runtimesystem :: normalFunctionCall" << endl;
  assert(args.size()>=2);
  char* mem1 = args[0];
  char* mem2 = args[1];
 assert(mem1);
 assert(mem2);
  int size = 0;
  if (args.size()>2)
    size = strtol(args[2],NULL,10);
  char* end1 = NULL;
  char* end2 = NULL;
  for (char *iter = mem1; *iter != '\0'; ++iter) {
    end1 = iter;
  }
  for (char *iter2 = mem2; *iter2 != '\0'; ++iter2) {
    end2 = iter2;
  }
 assert(end1);
 assert(end2);
    
  // check if string1 and string2 overlap. Dont allow memcopy on such
  int sizeMem1 = (end1-mem1)+1;
  int sizeMem2 = (end2-mem2)+1;
  cerr << " >>> FunctionCall : Checking mem1=<" << mem1 << "> (" << 
    sizeMem1 << ")  and  mem2=<" << mem2 << "> ("<< sizeMem2 <<  
    ")   sizeOp: " << size << "  " << endl;
  if (end1 >= mem2 && mem1<=end2) {
    cerr << " >>>> Error : Memory regions overlap!   Size1: " << sizeMem1 << "  Size2: " << sizeMem2 << endl;
    callExit(filename, line, (char*)"Memory regions overlap");  
  } else if (size>0 && (size>sizeMem1 || size>sizeMem2)) {
    // make sure that if the strings do not overlap, they are both smaller than the amount of chars to copy
    cerr << " >>>> Error : Memcopy is invalid : size = " << size << "   Size1: " << sizeMem1 << "  Size2: " << sizeMem2 << endl;
    char* res1 = ((char*)"Invalid Operation,  operand1 size=");
    char* res2 = ((char*)"  operand2 size=");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,sizeMem1,res2,sizeMem2);
    callExit(filename, line, res);
  } else if (size==0) {
    // strcpy (mem1, mem2)
    // make sure that size of mem2 is <= mem1
    if (sizeMem1<sizeMem2) {
      cerr << " >>>> Error : Memcopy is invalid : size = " << size << "   Size1: " << sizeMem1 << "  Size2: " << sizeMem2 << endl;
      char* res1 = ((char*)"Invalid Operation,  operand1 size=");
      char* res2 = ((char*)"  operand2 size=");
      char* res3 = ((char*)"  operand3 =");
      int sizeInt = 3*sizeof(int);
      char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ strlen(res3)+ 1);
      sprintf(res,"%s%d%s%d%s%d",res1,sizeMem1,res2,sizeMem2,res3,size);
      callExit(filename, line, res);
    }
  }
  cerr << "No problem found!" << endl;
}

void 
RuntimeSystem::roseFunctionCall(int count, ...) {
  cerr << "Runtimesystem :: functionCall" << endl;
  // handle the parameters within this call
  va_list vl;
  va_start(vl,count);
  std::vector<char*> args;
  char* name = NULL;
  char* mangl_name = NULL;
  char* beforeStr = NULL;
  char* scope_name = NULL;
  char* filename = NULL;
  char* line=NULL;
  bool before=false;
  //cerr << "arguments : " <<  count << endl;
  for (int i=0;i<count;i++)    {
    char* val=  va_arg(vl,char*);
    //cerr << " ... " << val << endl;
    if (i==0) name = val;
    else if (i==1) mangl_name =  val;
    else if (i==2) scope_name =  val;
    else if (i==3) {
      beforeStr = val;
      if (strcmp(beforeStr,"true")==0)
	before=true;
    }
    else if (i==4) filename =  val;
    else if (i==5) line = val;
    else {
      args.push_back(val);
    }
  }
  va_end(vl); 


  cerr << "roseFunctionCall :: " << name << " " << mangl_name << " " << before << endl;
  if (before && ( strcmp(name,"memcpy")==0 || 
		  strcmp(name ,"memmove")==0 || 
		  strcmp(name ,"strcpy")==0 ||
		  strcmp(name ,"strncpy")==0 ||
		  strcmp(name ,"strcat")==0
		  )) {
    handleNormalFunctionCalls(args, filename, line);
  } else {
    // we want to remember the varRefs that are passed via function calls to functions
    // if before ==true
    // add the current varRef (name) on stack
    // else remove from stack
    if (before) {
      RuntimeVariables* var = new RuntimeVariables(name,mangl_name);
      runtimeVariablesOnStack.push_back(var);
    }
    else {
      RuntimeVariables* var = runtimeVariablesOnStack.back();
      runtimeVariablesOnStack.pop_back();
      delete var;
    }
  }
}


// ***************************************** FUNCTION CALL *************************************



