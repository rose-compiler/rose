//! Translator interface
//! TODO: add stream operators for input and output
#include <vector>
#include <string>

template<class StatusType>
class Translator {

 public:
  virtual ~Translator() {};
  virtual void setOptions(int, char**) {};
  virtual void setOptions(const std::vector <std::string>& argvList) {};
  virtual void translate() { 
    frontend(); 
    midend(); 
    backend(); 
  }

  StatusType translate(int argc, char** argv) { 
    setOptions(argc,argv); 
    translate(); 
    return status();
  }

  StatusType translate(const std::vector <std::string>& argvList) { 
    setOptions(argvList); 
    translate(); 
    return status();
  }
 
  virtual StatusType status() { StatusType s(0); return s; }

 protected:
  virtual void frontend()=0;
  virtual void midend() { /* identity transformation is default */ } 
  virtual void backend()=0;
};

/*
  Translator c;
  c.translate(argc,argv);
OR
  Translator c;
  c.setOptions(argc,argv);
  c.translate();  
*/

