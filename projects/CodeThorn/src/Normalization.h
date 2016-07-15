namespace SPRAY {
class Normalization {
 public:
  static void normalizeAst(SgNode* root);
 private:
  static void convertAllForsToWhiles (SgNode* top);
  /* If the given statement contains any break statements in its body,
	add a new label below the statement and change the breaks into
	gotos to that new label.
  */
  static void changeBreakStatementsToGotos (SgStatement *loopOrSwitch);
};

} // end of namespace SPRAY
