#include "satire_legacy.h"

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

        std::cout << "collected " << pairs->size() << " expression pairs" << std::endl;
        return pairs;
    }
};

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

ExpressionPairSet *get_statement_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	std::string label = "PAG " + alias_type + "-alias " + pos;
	AliasPairAttribute* attr = dynamic_cast<AliasPairAttribute *>(stmt->getAttribute(label));
	return attr->getPairs();
}

int count_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	ExpressionPairSet *pairs = get_statement_alias_pairs(stmt, pos, alias_type);
    if (pairs == NULL)
	  return 0;
	return pairs->size();
}

std::string format_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	ExpressionPairSet *pairs = get_statement_alias_pairs(stmt, pos, alias_type);
    if (pairs == NULL)
      return "<undefined dfi>";
	std::stringstream str;
	ExpressionPairSet::iterator i;
	std::pair<SgNode*,SgNode*> *pair;
	for (i=pairs->begin(); i!=pairs->end(); i++) {
		pair = *i;
		str << "(" << Ir::fragmentToString(pair->first) << "," << Ir::fragmentToString(pair->second) << "), ";
	}
	return str.str();
}

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


namespace SATIrE {
    namespace MAIN_ANALYSIS {
        class ShapeImplementation : public Implementation {
        protected:
            ExpressionPairSet *expressionPairs;

        public:
            void annotateProgram(Program *program) {

                ShapeAnalyzerOptions *opt = (ShapeAnalyzerOptions*)program->options;

                /* Handle command line option for alias annotation. */
                if (opt->aliasesAnnotate()) {
                     cout << "annotating AST..." << std::endl;
                     TimingPerformance timer("Annotate AST with alias pairs:");

                     /* Extract all Pairs of Expressions from the Program so that they
                      * can be compared for aliasing. */
                     ExpressionCollector ec;
                     expressionPairs = ec.getExpressionPairs(program->astRoot);
                   
                     /* Add Alias Pairs to AST */
                     AliasPairsAnnotator<DFI_STORE> annotator((DFI_STORE)analysisInfo, expressionPairs);
                     if (program->options->analysisWholeProgram()) {
                         annotator.traverse(program->astRoot, preorder);
                     } else {
                         annotator.traverseInputFiles(program->astRoot, preorder);
                     }
                }

                // pass on to parent
                Implementation::annotateProgram(program);
            }

            void outputAnnotatedProgram(Program *program) {

                ShapeAnalyzerOptions *opt = (ShapeAnalyzerOptions*)program->options;

                if (opt->aliasesOutputText()) {
                    AliasPairsTextPrinter<DFI_STORE> p((DFI_STORE)analysisInfo, expressionPairs);
                    p.traverseInputFiles(program->astRoot, preorder);
                }
                
                if (opt->getaliasesOutputSource().length() > 0) {
                //    AliasPairsCommentAnnotator<DFI_STORE> ca((DFI_STORE)analysisInfo, expressionPairs);
		            //    ca.traverseInputFiles(program->astRoot, preorder);
		            //    ast_root->unparse();
                }

                // pass on to parent
                Implementation::outputAnnotatedProgram(program);
            }
        };
    }
}

