#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H

/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
class RuntimeSystem  {
 private:
  // a map of all arrays that were created
  std::map<std::string, int> arrays;

 public:
  RuntimeSystem(){
    arrays.clear();
  };
  virtual ~RuntimeSystem(){
    arrays.clear();
  };

  // create array and store its size
  void roseCreateArray(std::string name, int size);
  // check if array is out of bounds
  void roseArrayAccess(std::string name, int pos);

};

// static RuntimeSystem that is accessible by all source files
static RuntimeSystem* runtimeSystem = new RuntimeSystem();
#endif

