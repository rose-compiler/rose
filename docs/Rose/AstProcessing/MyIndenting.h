

class MyIndentLevel {
 public:
  MyIndentLevel():level(0) {
  }
  unsigned int level;
};

class MyIndenting : public AstTopDownProcessing<MyIndentLevel> {
 protected:
  void virtual evaluateInheritedAttribute(SgNode* astNode);
 private:
  unsigned int tabSize;
};

