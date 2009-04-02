/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
//#include "rose.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
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

/* -----------------------------------------------------------
 * create array and store its size
 * -----------------------------------------------------------*/
void
RuntimeSystem::roseCreateArray(std::string name, int dimension, bool stack, long int sizeA, long int sizeB, std::string filename, int line ){
  if (oldFilename!=filename) {
    fileNr++;
    *myfile << fileNr << ": >>> Checking : " << findLastUnderscore(name) << " dim"<< dimension <<
      " - [" << sizeA << "][" << sizeB << "]  file : " << filename << " line : " << line << endl;
    if (oldFilename!="" && violation==false) {
      cerr << "No violation in this file found. Check this! " << endl;
      exit(1);
    }
    cerr << " Setting violation to false ! " << endl;
    violation=false;
    violationNr=0;
  }

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
    }
  } 
  if (it==arrays1D.end() && it2==arrays2D.end()) {
    std::cout << endl;
    std::cerr << " >>> No such array was created. Can't access it. " << filename << "  l: " << line << endl;
  }

}

