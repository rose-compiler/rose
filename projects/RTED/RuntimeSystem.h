#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Contains variable names for variables that are passed via functions
 * -----------------------------------------------------------*/
class RuntimeVariables {
 public:
  char* name;
  char* mangled_name;
  RuntimeVariables(char* n, char* m_n) {
    name = n;
    mangled_name=m_n;
  };
  virtual ~RuntimeVariables(){}
};


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Store information about arrays and their sizes
 * -----------------------------------------------------------*/
class Array2D {
 public:
  int size1;
  int* array;
  Array2D(int s1) {
    size1=s1;
    array=new int[s1];
  }
  void allocate(int s1,int s2) {
    array[s1]=s2;
  }
  int getSize(int s1) {
    return array[s1];
  }
  virtual ~Array2D(){}
};



/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
class RuntimeSystem  {
 public:
  static RuntimeSystem* Instance();
 protected:
  RuntimeSystem();
  RuntimeSystem(const RuntimeSystem&);
  RuntimeSystem& operator= (const RuntimeSystem&);
 private:
  static RuntimeSystem* pinstance;
  bool arrayDebug;

  void handleNormalFunctionCalls(std::vector<char*>& args, char* filename, char* line);

  std::vector<RuntimeVariables*> runtimeVariablesOnStack;
  char* findVariablesOnStack(char* name);

  void callExit(char* filename, char* line, char* reason);
  
  char* findLastUnderscore(char* s);
  // a map of all arrays that were created
  std::map<std::string, int> arrays1D;
  std::map<std::string, Array2D*> arrays2D;
  bool violation;
  std::ofstream* myfile;
  std::string filename;
  std::string oldFilename;
  int fileNr;
  int violationNr;
 public:
  virtual ~RuntimeSystem(){
    arrays1D.clear();
  };

  std::string resBool(bool val);
  void roseRtedClose();

  // create array and store its size
  void roseCreateArray(char* name, int dimension, bool stack, 
		       long int sizeA, long int sizeB, char* filename, char* line);
  // check if array is out of bounds
  void roseArrayAccess(char* name, int posA, int posB, char* filename, char* line);
  
  void  roseFunctionCall(int count, ...);
  template<typename T> std::string roseConvertToString(T t);
  char* roseConvertIntToString(int t);
};

static RuntimeSystem *runtimeSystem = RuntimeSystem::Instance();
#endif

