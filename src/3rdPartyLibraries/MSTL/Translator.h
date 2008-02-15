//! Translator interface
//! TODO: add stream operators for input and output
template<class StatusType>
class Translator {

 public:
  virtual ~Translator() {};
  virtual void setOptions(int argc,char** argv) {};
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

