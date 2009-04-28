#include "satire_legacy.h"

#include "AliasStatistics.C"
extern AliasStatistics *stats;

typedef std::set<std::pair<SgExpression*,SgExpression*>*> ExprPairSet;

class AliasPairAttribute : public AstAttribute {
private: 
  ExprPairSet *pairs;
public:
  AliasPairAttribute(ExprPairSet *_pairs) { pairs = _pairs; }
  ExprPairSet *getPairs() {return pairs;}
};


class ExprPairCollector {
public:
    static ExprPairSet* getPointerExprPairs() {
        // get all relevant expressions, ie. pointer expressions
        CFG *icfg = get_global_cfg();
        std::vector<SgExpression *> pointer_exprs;

        std::vector<SgExpression *>::iterator e;
        for (e = icfg->numbers_exprs.begin(); e != icfg->numbers_exprs.end(); ++e) {
            if (o_is_pointer_expression(*e)) {
                pointer_exprs.push_back(*e);
            }
        }

        // combine into pairs (only above the diagonal)
        ExprPairSet *pairs = new ExprPairSet();
        std::vector<SgExpression *>::iterator i,j;
        for (i=pointer_exprs.begin(); i!=pointer_exprs.end(); i++) {
            j=i;
            for (j++; j!=pointer_exprs.end(); j++) {
                pairs->insert(new std::pair<SgExpression*,SgExpression*>(*i,*j));
            }
        }
        
        // std::cout << "collected " << pairs->size() << " pointer expression pairs" << std::endl;
        return pairs;
    }
};

/* AliasPairsAnnotator puts alias pairs into AST */
template<typename DFI_STORE_TYPE>
class AliasPairsAnnotator : public StatementAttributeTraversal<DFI_STORE_TYPE> {
private:
    ExprPairSet *candidate_pairs;
  
public:
  AliasPairsAnnotator(DFI_STORE store_, ExprPairSet *candidate_pairs_) : StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {
    candidate_pairs = candidate_pairs_;
  }
  virtual ~AliasPairsAnnotator() {}

protected:
  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {
    computeNumberOfNnhGraphs(stmt);
    addAliasPairsAttribute(stmt, statementPreMustAliases(stmt),   "pre", "must");
    addAliasPairsAttribute(stmt, statementPostMustAliases(stmt), "post", "must");
    addAliasPairsAttribute(stmt, statementPreMayAliases(stmt),    "pre", "may");
    addAliasPairsAttribute(stmt, statementPostMayAliases(stmt),  "post", "may");
  }

  virtual void computeNumberOfNnhGraphs(SgStatement *stmt) {
    o_SrwNnhPair gpair = o_extract_graphs(o_{DFI}_drop(get_statement_post_info(this->store,stmt)));
    o_ShapeGraphList graphs = o_SrwNnhPair_select_2(gpair);
    stats->addGraphCount(o_ShapeGraphList_length(graphs));
  }

  ExprPairSet *statementPreMustAliases(SgStatement *stmt) {
    ExprPairSet *aliases = computeMustAliasPairs(get_statement_pre_info(this->store,stmt));
    stats->addPreMustAliasCount(aliases->size());
    return aliases;
  }
  ExprPairSet *statementPostMustAliases(SgStatement *stmt) {
    ExprPairSet *aliases = computeMustAliasPairs(get_statement_post_info(this->store,stmt));
    stats->addPostMustAliasCount(aliases->size());
    return aliases;
  }
  ExprPairSet *statementPreMayAliases(SgStatement *stmt) {
    ExprPairSet *aliases = computeMayAliasPairs(get_statement_pre_info(this->store,stmt));
    stats->addPreMayAliasCount(aliases->size());
    return aliases;
  }
  ExprPairSet *statementPostMayAliases(SgStatement *stmt) {
    ExprPairSet *aliases = computeMayAliasPairs(get_statement_post_info(this->store,stmt));
    stats->addPostMayAliasCount(aliases->size());
    return aliases;
  }

  ExprPairSet *computeMustAliasPairs(carrier_type_o(CARRIER_TYPE) sg) { 
    if (sg == NULL)
      return NULL;
    ExprPairSet *aliases = new ExprPairSet();
    ExprPairSet::iterator i;
    std::pair<SgExpression*,SgExpression*> *pair;
    for (i=candidate_pairs->begin(); i!=candidate_pairs->end(); i++) {
      pair = *i;
      if (o_is_must_alias(pair->first, pair->second, o_shape_carrier_to_shapegraphset(sg))) {
        aliases->insert(pair);
      }
    }
    return aliases;
  }
  
  ExprPairSet *computeMayAliasPairs(carrier_type_o(CARRIER_TYPE) sg) {
    if (sg == NULL)
      return NULL;
    ExprPairSet *aliases = new ExprPairSet();
    ExprPairSet::iterator i;
    std::pair<SgExpression*,SgExpression*> *pair;
    for (i=candidate_pairs->begin(); i!=candidate_pairs->end(); i++) {
      pair = *i;
      if (o_is_may_alias(pair->first, pair->second, o_shape_carrier_to_shapegraphset(sg))) {
        aliases->insert(pair);
      }
    }
    return aliases;
  }

  void addAliasPairsAttribute(SgStatement *node, ExprPairSet *exprs, std::string pos, std::string alias_type) {
    std::string label = "PAG " + alias_type + "-alias " + pos;
    node->setAttribute(label, new AliasPairAttribute(exprs));
  }
};

ExprPairSet *get_statement_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	std::string label = "PAG " + alias_type + "-alias " + pos;
	AliasPairAttribute* attr = dynamic_cast<AliasPairAttribute *>(stmt->getAttribute(label));
	return attr->getPairs();
}

int count_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	ExprPairSet *pairs = get_statement_alias_pairs(stmt, pos, alias_type);
    if (pairs == NULL)
	  return 0;
	return pairs->size();
}

std::string format_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	ExprPairSet *pairs = get_statement_alias_pairs(stmt, pos, alias_type);
    if (pairs == NULL)
      return "<undefined dfi>";
	std::stringstream str;
	ExprPairSet::iterator i;
	std::pair<SgExpression*,SgExpression*> *pair;
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
    ExprPairSet *pairs;
public:
    AliasPairsTextPrinter(DFI_STORE_TYPE store, ExprPairSet *_pairs) : DfiTextPrinter<DFI_STORE>(store) {
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
    ExprPairSet *pairs;
public:
    AliasPairsCommentAnnotator(DFI_STORE_TYPE store, ExprPairSet *_pairs) : DfiCommentAnnotator<DFI_STORE>(store) {
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
            ExprPairSet *pointerExpressionPairs;

        public:
            void annotateProgram(Program *program) {

                ShapeAnalyzerOptions *opt = (ShapeAnalyzerOptions*)program->options;

                /* Handle command line option for alias annotation. */
                if (opt->aliasesAnnotate()) {
                    cout << "annotating AST ...";
                    TimingPerformance timer("Annotate AST with alias pairs:");

                    /* get pairs of all pointer expressions as candidates for alias testing */
                    pointerExpressionPairs = ExprPairCollector::getPointerExprPairs();
                    stats->setExprPairCount(pointerExpressionPairs->size());

                    /* Add Alias Pairs to AST */
                    AliasPairsAnnotator<DFI_STORE> annotator((DFI_STORE)analysisInfo, pointerExpressionPairs);
                    if (program->options->analysisWholeProgram()) {
                        annotator.traverse(program->astRoot, preorder);
                    } else {
                        annotator.traverseInputFiles(program->astRoot, preorder);
                    }
                    cout << " done" << std::endl;
                }
                // pass on to parent
                Implementation::annotateProgram(program);
            }

            void outputAnnotatedProgram(Program *program) {

                ShapeAnalyzerOptions *opt = (ShapeAnalyzerOptions*)program->options;

                if (opt->aliasesOutputText()) {
                    AliasPairsTextPrinter<DFI_STORE> p((DFI_STORE)analysisInfo, pointerExpressionPairs);
                    p.traverseInputFiles(program->astRoot, preorder);
                }
                
                if (opt->getaliasesOutputSource().length() > 0) {
                //    AliasPairsCommentAnnotator<DFI_STORE> ca((DFI_STORE)analysisInfo, pointerExpressionPairs);
		            //    ca.traverseInputFiles(program->astRoot, preorder);
		            //    ast_root->unparse();
                }

                if (opt->getaliasStatisticsFile().length() > 0) {
                    cout << "writing alias statistics to " << opt->getaliasStatisticsFile() << std::endl;
                    stats->writeFile(opt->getaliasStatisticsFile().c_str());
                }
                

                // pass on to parent
                Implementation::outputAnnotatedProgram(program);
            }
        };
    }
}

