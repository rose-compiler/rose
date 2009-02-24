#ifndef KEEPATTRIBUTE_IS_DEFINED
#define KEEPATTRIBUTE_IS_DEFINED



/*! 
  This attribute is added to all AST nodes which we want to keep in the slice of a program code.
*/
class KeepAttribute : public AstAttribute{

 public:
  KeepAttribute(bool k):keep(k){}

  void setAttribute(bool b){ keep = b; }
  bool getAttribute(){ return keep;}
  void operator << (const char* a) {printf("The keep attribute: %s",a); }

 private:
  bool keep;

};

#endif
