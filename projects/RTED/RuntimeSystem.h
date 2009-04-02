#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <iostream>
#include <string>
#include <fstream>

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


// static RuntimeSystem that is accessible by all source files
//static RuntimeSystem* runtimeSystem = new RuntimeSystem();

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

  std::string findLastUnderscore(std::string& s);
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

  std::string resBool(bool val) {                                               
    if (val)                                                                      
      return "true";                                                           
    return "false";                                                          
  }

  void roseRtedClose() {
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


  // create array and store its size
  void roseCreateArray(std::string name, int dimension, bool stack, long int sizeA, long int sizeB, std::string filename, int line);
  // check if array is out of bounds
  void roseArrayAccess(std::string name, int posA, int posB, std::string filename, int line);

};

static RuntimeSystem *runtimeSystem = RuntimeSystem::Instance();
#endif

