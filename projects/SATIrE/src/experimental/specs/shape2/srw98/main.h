/* -*- C++ -*-
 * use this file for additional declarations
 * necessary for your analysis. */

extern "C" void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx);
extern "C" void my_o_Node_print_fp(FILE * fp, o_Node node);
extern "C" int gdl_write_shapegraph_fp(FILE *fp, char *name, int n_graphs,  char *attrib, o_ShapeGraph sg);

#include "AnalyzerOptions.h"

class ShapeAnalyzerOptions : public AnalyzerOptions {
public:
    ShapeAnalyzerOptions() : AnalyzerOptions() {
      gdlFoldGraphsOff();
      gdlShowIndividualGraphsOff();
      gdlShowSummaryGraphOn();
	  setGraphStatisticsFile(NULL);
    }
    virtual ~ShapeAnalyzerOptions() {}

    void aliasTextOutputOn()  { _aliasTextOutput = true;  }
    void aliasTextOutputOff() { _aliasTextOutput = false; }
    bool aliasTextOutput()    { return _aliasTextOutput;  }

    void aliasSourceOutputOn()  { _aliasSourceOutput = true;  }
    void aliasSourceOutputOff() { _aliasSourceOutput = false; }
    bool aliasSourceOutput()    { return _aliasSourceOutput;  }

    void gdlFoldGraphsOn()  { _gdlFoldGraphs = true;  }
    void gdlFoldGraphsOff() { _gdlFoldGraphs = false; }
    bool gdlFoldGraphs()    { return _gdlFoldGraphs;  }

    void gdlShowSummaryGraphOn()  { _gdlShowSummaryGraph = true;  }
    void gdlShowSummaryGraphOff() { _gdlShowSummaryGraph = false; }
    bool gdlShowSummaryGraph()    { return _gdlShowSummaryGraph;  }

    void gdlShowIndividualGraphsOn()  { _gdlShowIndividualGraphs = true;  }
    void gdlShowIndividualGraphsOff() { _gdlShowIndividualGraphs = false; }
    bool gdlShowIndividualGraphs()    { return _gdlShowIndividualGraphs;  }

    char *graphStatisticsFile() { return _graphStatisticsFile; }
    void setGraphStatisticsFile(char *fn) { _graphStatisticsFile = fn; }

    virtual std::string getOptionsInfo() {
      return AnalyzerOptions::getOptionsInfo() + 
        " Custom options:\n" 
        "   --output-alias                output the alias pairs for each statement on stdout\n"
        "   --output-alias-annotation     add alias pairs to each statement in generated source file\n"
        "   --individualgraphs            output all individual graphs in gdl\n"
        "   --no-individualgraphs         do not output all individual graphs in gdl [default]\n"
        "   --summarygraph                output summary graph in gdl [default]\n"
        "   --no-summarygraph             do not output summary graph in gdl\n"
        "   --foldgraphs                  fold all gdl graphs initially\n"
		"\n"
		"   --output-graph-statistics=<FILENAME>  write graph statistics to <FILENAME>\n";
    }

protected:
    bool _aliasTextOutput;
    bool _aliasSourceOutput;
    bool _gdlFoldGraphs;
    bool _gdlShowSummaryGraph;
    bool _gdlShowIndividualGraphs;
	char *_graphStatisticsFile;
};

class ShapeCommandLineParser : public CommandLineParser {
public:
    int handleOption(AnalyzerOptions *cl, int i, int argc, char **argv) {
      if (i >= argc)
        return 0;

      ShapeAnalyzerOptions *scl = dynamic_cast<ShapeAnalyzerOptions*>(cl);

      if (optionMatch(argv[i], "--output-alias")) {
        scl->aliasTextOutputOn();
      } else if (optionMatch(argv[i], "--output-alias-annotation")) {
        scl->aliasSourceOutputOn();
      } else if (optionMatch(argv[i], "--individualgraphs")) {
        scl->gdlShowIndividualGraphsOn();
      } else if (optionMatch(argv[i], "--no-individualgraphs")) {
        scl->gdlShowIndividualGraphsOff();
      } else if (optionMatch(argv[i], "--summarygraph")) {
        scl->gdlShowSummaryGraphOn();
      } else if (optionMatch(argv[i], "--no-summarygraph")) {
        scl->gdlShowSummaryGraphOff();
      } else if (optionMatch(argv[i], "--foldgraphs")) {
        scl->gdlFoldGraphsOn();
	  } else if (optionMatchPrefix(argv[i],"--output-graph-statistics=")) {
	    scl->setGraphStatisticsFile(strdup(argv[i]+26)); // FIXME prefixlength should be protected to remove constant 26
      } else {
        return CommandLineParser::handleOption(cl,i,argc,argv);
      }
      return 1;
    }
};

// -- new parts follow

typedef std::set<std::pair<SgNode*,SgNode*>*> ExpressionPairSet;

class AliasPairAttribute : public AstAttribute {
private: 
  ExpressionPairSet *pairs;
public:
  AliasPairAttribute(ExpressionPairSet *es) { pairs = es; }
  ExpressionPairSet *getPairs() {return pairs;}
};

/* ExpressionCollector.getExpressionPairs() returns all
 * pairs of expressions (e_i,e_j) s.t. i!=j and e <- expressions(Program)
 */

struct ltexpr {
    bool operator()(SgNode* a, SgNode* b) const {
        return Ir::fragmentToString(a) < Ir::fragmentToString(b);
    }
};

class ExpressionCollector {
public:
    ExpressionPairSet* getExpressionPairs(SgProject *projectNode) {

        class ExpressionCollectorTraversal : public AstSimpleProcessing {
            private:
                std::set<SgNode*,ltexpr> *es;

            public:
                std::set<SgNode*,ltexpr>* collectExpressions(SgProject* projectNode) {
                    es = new std::set<SgNode*,ltexpr>();
                    traverseInputFiles(projectNode, preorder);
                    return es;
                }

            protected:
                virtual void visit(SgNode *node) {
                    switch (node->variantT()) {
                        case V_SgArrowExp:
                        case V_SgDotExp:
                        case V_SgVarRefExp:
                            es->insert(node);
                            break;
                        default:
                            //std::cout << node->class_name() << std::endl;
                            break;
                        }
                }
        };

        ExpressionCollectorTraversal trav;
        std::set<SgNode*,ltexpr> *exprs = trav.collectExpressions(projectNode);

        // generate all (relevant) permutations of collected expressions
        // under commutativity of expressions (a,b) == (b,a)
        ExpressionPairSet *pairs = new ExpressionPairSet();

        std::set<SgNode*,ltexpr>::iterator i,j;
        for (i=exprs->begin(); i!=exprs->end(); i++) {
            j=i;
            for (j++; j!=exprs->end(); j++) {
                SgNode *a,*b,*tmp;
                a = *i;
                b = *j;
                if (Ir::fragmentToString(a).length() < Ir::fragmentToString(b).length()) {
                    // make sure that the longer expression is on the left (to avoid duplicates)
                    tmp = a;
                    a = b;
                    b = tmp;
                }
                pairs->insert(new std::pair<SgNode*,SgNode*>(a,b));
            }
        }
        
        delete exprs;
        return pairs;
    }
};

std::string format_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type);

/* AliasPairsAnnotator puts alias pairs into AST */
template<typename DFI_STORE_TYPE>
class AliasPairsAnnotator : public StatementAttributeTraversal<DFI_STORE_TYPE> {
private:
  ExpressionPairSet *candidate_pairs;
  
public:
  AliasPairsAnnotator(DFI_STORE store_, ExpressionPairSet *candidate_pairs_) : StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {
    candidate_pairs = candidate_pairs_;
  }
  virtual ~AliasPairsAnnotator() {}

protected:
  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {
    addAliasPairsAttribute(stmt, statementPreMustAliases(stmt),   "pre", "must");
    addAliasPairsAttribute(stmt, statementPostMustAliases(stmt), "post", "must");
    addAliasPairsAttribute(stmt, statementPreMayAliases(stmt),    "pre", "may");
    addAliasPairsAttribute(stmt, statementPostMayAliases(stmt),  "post", "may");
  }

  ExpressionPairSet *statementPreMustAliases(SgStatement *stmt) {
    return computeMustAliasPairs(get_statement_pre_info(this->store,stmt));
  }
  ExpressionPairSet *statementPostMustAliases(SgStatement *stmt) {
    return computeMustAliasPairs(get_statement_post_info(this->store,stmt));
  }
  ExpressionPairSet *statementPreMayAliases(SgStatement *stmt) {
    return computeMayAliasPairs(get_statement_pre_info(this->store,stmt));
  }
  ExpressionPairSet *statementPostMayAliases(SgStatement *stmt) {
    return computeMayAliasPairs(get_statement_post_info(this->store,stmt));
  }

  ExpressionPairSet *computeMustAliasPairs(carrier_type_o(CARRIER_TYPE) sg) { 
    if (sg == NULL)
      return NULL;
    ExpressionPairSet *aliases = new ExpressionPairSet();
    ExpressionPairSet::iterator i;
    std::pair<SgNode*,SgNode*> *pair;
    for (i=candidate_pairs->begin(); i!=candidate_pairs->end(); i++) {
      pair = *i;
      if (o_is_must_alias(pair->first, pair->second, o_shape_carrier_to_shapegraphset(sg))) {
        aliases->insert(pair);
      }
    }
    return aliases;
  }
  
  ExpressionPairSet *computeMayAliasPairs(carrier_type_o(CARRIER_TYPE) sg) {
    if (sg == NULL)
      return NULL;
    ExpressionPairSet *aliases = new ExpressionPairSet();
    ExpressionPairSet::iterator i;
    std::pair<SgNode*,SgNode*> *pair;
    for (i=candidate_pairs->begin(); i!=candidate_pairs->end(); i++) {
      pair = *i;
      if (o_is_may_alias(pair->first, pair->second, o_shape_carrier_to_shapegraphset(sg))) {
        aliases->insert(pair);
      }
    }
    return aliases;
  }

  void addAliasPairsAttribute(SgStatement *node, ExpressionPairSet *exprs, std::string pos, std::string alias_type) {
    std::string label = "PAG " + alias_type + "-alias " + pos;
    node->setAttribute(label, new AliasPairAttribute(exprs));
  }
};

/* AliasPairsTextPrinter prints alias pairs from AST to console */
template <typename DFI_STORE_TYPE>
class AliasPairsTextPrinter : public DfiTextPrinter<DFI_STORE_TYPE> {
private:
    ExpressionPairSet *pairs;
public:
    AliasPairsTextPrinter(DFI_STORE_TYPE store, ExpressionPairSet *_pairs) : DfiTextPrinter<DFI_STORE>(store) {
        pairs = _pairs;
    }
  
protected:
    virtual void handleStmtDfi(SgStatement* stmt, std::string _unused1, std::string _unused2) {
        std::string preInfo =  AliasPairsTextPrinter<DFI_STORE_TYPE>::getPreInfo(stmt);
        std::string postInfo = AliasPairsTextPrinter<DFI_STORE_TYPE>::getPostInfo(stmt);
        std::string stmt_str = Ir::fragmentToString(stmt);
        
        std::cout << this->currentFunction() << ": " << "// pre must-aliases : " << 
          format_alias_pairs(stmt, "pre", "must") << std::endl;
        
        std::cout << this->currentFunction() << ": " << "// pre may-aliases : " << 
          format_alias_pairs(stmt, "pre", "may") << std::endl;
        
        std::cout << this->currentFunction() << ": " << stmt_str << std::endl;
        
        std::cout << this->currentFunction() << ": " << "// post must-aliases : " << 
          format_alias_pairs(stmt, "post", "must") << std::endl;
        
        std::cout << this->currentFunction() << ": " << "// post may-aliases : " << 
          format_alias_pairs(stmt, "post", "may") << std::endl << std::endl;
    }
};

/* AliasPairsCommentAnnotator writes alias pairs from AST to sourcecode */
template <typename DFI_STORE_TYPE>
class AliasPairsCommentAnnotator : public DfiCommentAnnotator<DFI_STORE_TYPE> {
private:
    ExpressionPairSet *pairs;
public:
    AliasPairsCommentAnnotator(DFI_STORE_TYPE store, ExpressionPairSet *_pairs) : DfiCommentAnnotator<DFI_STORE>(store) {
        pairs = _pairs;
    }
protected:
    virtual void handleStmtDfi(SgStatement* stmt, std::string _unused1, std::string _unused2) {
        AliasPairsCommentAnnotator<DFI_STORE_TYPE>::addCommentBeforeNode("// pre must-aliases : " + format_alias_pairs(stmt, "pre", "must"), stmt);
        AliasPairsCommentAnnotator<DFI_STORE_TYPE>::addCommentBeforeNode("// pre may-aliases : "  + format_alias_pairs(stmt, "pre", "may"), stmt);
        AliasPairsCommentAnnotator<DFI_STORE_TYPE>::addCommentAfterNode("// post must-aliases : " + format_alias_pairs(stmt, "post","must"), stmt);
        AliasPairsCommentAnnotator<DFI_STORE_TYPE>::addCommentAfterNode("// post may-aliases : "  + format_alias_pairs(stmt, "post","may"), stmt);
    }
};

