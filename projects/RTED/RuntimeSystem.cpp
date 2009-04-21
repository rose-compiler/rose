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
  //myfile->close();
  //myfile = new std::ofstream("result-array.txt",std::ios::app);
  violation=false;
  violationNr=0;
  filename="";
  oldFilename="";
  fileNr=0;

}


std::string
RuntimeSystem::findLastUnderscore(std::string& s) {
  string name = "";
  int pos = s.rfind("_");
  if (pos!=(int)std::string::npos) {
    name = s.substr(pos,s.length());
  }
  return name;
}

void
RuntimeSystem::callExit() {
  // violation Found ... dont execute it - exit normally
  cerr << "violation found. exit program." << endl;
  exit(0);
}

/* -----------------------------------------------------------
 * create array and store its size
 * -----------------------------------------------------------*/
void
RuntimeSystem::roseCreateArray(std::string name, int dimension, bool stack, long int sizeA, long int sizeB, std::string filename, int line ){
#if 0
  if (oldFilename!=filename) {
    fileNr++;
    *myfile << fileNr << ": >>> Checking : " << findLastUnderscore(name) << " dim"<< dimension <<
      " - [" << sizeA << "][" << sizeB << "]  file : " << filename <<  " line : " << line << endl;
    if (oldFilename!="" && violation==false) {
      cerr << "roseCreateArray :: No violation in this file found. Check this! " << endl;
      exit(1);
    }
    cerr << " Setting violation to false ! " << endl;
    violation=false;
    violationNr=0;
  }
#endif
  cout << fileNr << ": >>> Called : roseCreateArray : " << findLastUnderscore(name) << " dim"<< dimension <<
    " - [" << sizeA << "][" << sizeB << "] file : " << filename << " line : " << line  << endl;
  oldFilename=filename;
  if (dimension==1) {
    arrays1D[name]=sizeA;
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
      cerr << "..    Expanding 2nd-run 2Dim array - sizeA : " << sizeA << "  sizeB : " << sizeB << endl;
      if (sizeA<0 || sizeA>=totalsize) {
	cerr << " Violation detected :  Array too small to allocate more memory " << endl;
	violation=true;
	violationNr++;
	callExit();
      } else {
	cerr << " >>> CREATING Array : arr ["<<totalsize<<"]["<<sizeB<<"]"<< "  alloc : ["<<sizeA<<"]="<<sizeB<<endl;
	array->allocate(sizeA,sizeB); //arr[sizeA][sizeB]
      }
    } else {

      // new array
      array = new Array2D(sizeA);// ptr [][] = malloc (20)   20 == totalsize
      arrays2D[name]=array;
      cerr << ".. Creating 2Dim array - size : " << sizeA << endl;
      if (sizeB!=-1) {
	// expand this stack array
	for (int i=0;i<sizeA;++i)
	  array->allocate(i,sizeB); //arr[i][sizeB]
	cerr << "..    Expanding 2Dim array - sizeA : " << sizeA << "  sizeB : " << sizeB << endl;
      }
    }
  }
}

/* -----------------------------------------------------------
 * check if array is out of bounds
 * -----------------------------------------------------------*/
void
RuntimeSystem::roseArrayAccess(std::string name, int posA, int posB, std::string filename, int line){
  filename=filename;
  cout << "    Called : roseArrayAccess : " << findLastUnderscore(name) << " ... ";

  // check the stack if the variable is part of a function call
  std::string mangl_name=findVariablesOnStack(name);  
  if (mangl_name!="not found")
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
      cout << "  >>>>>> Violation detected : Array size: " << size << " accessing: " << posA <<
	"  in : " << filename << "  line: " << line << endl;
      *myfile << "  >>>>>> Violation detected : Array size: " << size << " accessing: " << posA <<
	"  in : " << filename << "  line: " << line << endl;
      violation=true;
      violationNr++;
      callExit();
    }
  }

  std::map<std::string, Array2D*>::const_iterator it2 = arrays2D.find(name);
  if (it2!=arrays2D.end()) {
    Array2D* array = it2->second;
    int sizeA = array->size1;
    int sizeB = array->getSize(posA);
    cout << "  Found 2Dim array :  size: [" << sizeA << "][" << sizeB << "]  pos: [" << posA << "][" << posB << "]" <<endl;
    // allow arr[posA][posB] && arr[posA]  both 2Dim!
    if ((posA>=sizeA || posA<0) || posB>=sizeB || posB<0) {
      cout << "  >>>>>> Violation detected : Array size: [" << sizeA << "]["<<sizeB<<"] accessing: ["  << posA <<
        "][" << posB << "]  in : " << filename << "  line: " << line << endl;
      *myfile << "  >>>>>> Violation detected : Array size: [" << sizeA << "]["<<sizeB<<"] accessing: ["  << posA <<
        "][" << posB << "]  in : " << filename << "  line: " << line << endl;
      violation=true;
      violationNr++;
      callExit();
    }
  } 
  if (it==arrays1D.end() && it2==arrays2D.end()) {
    std::cout << endl;
    std::cerr << " >>> No such array was created. Can't access it. " << filename << "  l: " << line << endl;
    exit(1);
  }

}

#if 0
void 
RuntimeSystem::roseFunctionCall(std::string name, std::string mangl_name, bool before) {
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
  cerr << "roseFunctionCall :: " << name << " " << mangl_name << " " << before << endl;
}
#endif

void 
RuntimeSystem::roseFunctionCall(int count, ...) {
  cerr << "Runtimesystem :: functionCall" << endl;
  //printf("returning : %s\n",test); 
  va_list vl;
  va_start(vl,count);
  std::vector<std::string> args;
  string name = "";
  string mangl_name = "";
  string beforeStr = "";
  string scope_name = "";
  bool before=false;
  cerr << "arguments : " <<  count << endl;
  for (int i=0;i<count;i++)    {
    string val= (string) va_arg(vl,char*);
    cerr << " ... " << val << endl;
    if (i==0) name = val;
    else if (i==1) mangl_name = val;
    else if (i==2) scope_name = val;
    else if (i==3) {
      beforeStr = val;
      if (beforeStr=="true")
	before=true;
    } else {
      args.push_back(val);
    }
  }
  va_end(vl); 

  if (name=="memcpy") {
    cerr << "Runtimesystem :: found memcopy" << endl;
    string mem1 = args[0];
    string mem2 = args[1];
    int size = strtol(args[2].c_str(),NULL,10);
    cerr << " Checking <" << mem1 << ">  and <" << mem2 << ">    size: " << size << "  " << args[2]<< endl;
    cerr << "No buffer overflow" << endl;
//    callExit();
  } else {

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

  cerr << "roseFunctionCall :: " << name << " " << mangl_name << " " << before << endl;
}

std::string
RuntimeSystem::findVariablesOnStack(std::string name) {
  std::string mang_name = "not found";
  std::vector<RuntimeVariables*>::const_reverse_iterator stack = 
    runtimeVariablesOnStack.rbegin();
  for (;stack!=runtimeVariablesOnStack.rend();++stack) {
    RuntimeVariables* rv = *stack;
    if (name==rv->name) {
      mang_name=rv->mangled_name;
    }
  }
  return mang_name;
}



