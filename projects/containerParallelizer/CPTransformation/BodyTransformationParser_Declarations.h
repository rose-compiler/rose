class DerefExpAttribute {
 public:
  DerefExpAttribute():withinFunctionCallExp(false),isPointerDerefExp(false) {}
  bool withinFunctionCallExp;
  bool isPointerDerefExp;
};

class DerefExpSynAttribute {
 public:
  DerefExpSynAttribute():isIteratorDeref(false),varNameMatches(false),lhsFunctionCallIsIteratorAccess(false) {}
  bool isIteratorDeref;
  bool varNameMatches;
  bool lhsFunctionCallIsIteratorAccess;
};
