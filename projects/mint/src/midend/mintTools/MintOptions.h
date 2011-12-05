

#include <iostream>
#include <string>
#include <vector>

#include <AstInterface.h>


class MintOptions
{ 
 private:
  static MintOptions *inst;

  unsigned defaultblocksize;

 public:

 MintOptions() : opts() {}

  static void setInternalCommandLineArgs(int &argc, char* argv[]);  

  std::vector<std::string> opts; // So its .end() method is accessible                                                                         
  static MintOptions* GetInstance () ;

  void SetMintOptions( const std::vector<std::string>& argvList);

  bool HasOption( const std::string& opt);

  void PrintUsage(std::ostream& stream) const ;

  unsigned GetDefaultBlockSize() const { return defaultblocksize; }

  int GetOptimizationType();

  bool optimize();
  bool linearize();
  bool isSlidingOpt();//not being used

  bool isRegisterOpt();
  bool isSharedOpt();
  bool isUnrollOpt();
  bool useSameIndex();
  bool isL1Preferred();
  bool isSwapOpt();//not being used

  std::vector<std::string>::const_iterator GetOptionPosition( const std::string& opt); 

  static int getNumberOfSharedMemPlanes();
};

