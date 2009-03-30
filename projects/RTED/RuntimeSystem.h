#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H

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
 private:
  // a map of all arrays that were created
  std::map<std::string, int> arrays1D;
  std::map<std::string, Array2D*> arrays2D;

 public:
  RuntimeSystem(){
    arrays1D.clear();
  };
  virtual ~RuntimeSystem(){
    arrays1D.clear();
  };

  // create array and store its size
  void roseCreateArray(std::string name, int dimension, bool stack, long int sizeA, long int sizeB);
  // check if array is out of bounds
  void roseArrayAccess(std::string name, int posA, int posB, std::string filename, int line);

};

// static RuntimeSystem that is accessible by all source files
static RuntimeSystem* runtimeSystem = new RuntimeSystem();
#endif

