#ifndef H_PAG_ANALYSIS_MAIN
#define H_PAG_ANALYSIS_MAIN

#include <config.h>
#include "CFGTraversal.h"
#include "ProcTraversal.h"

#include "iterate.h"

#undef NDEBUG
#include "testcfg.h"
#include "CommandLineParser.h"

#include "StatementAttributeTraversal.h"
#include "AstSimpleProcessing.h"

#define doit(analysis) xdoit(analysis)
#define xdoit(analysis) analysis##_doit

#define stringize(analysis) xstr(analysis)
#define xstr(analysis) #analysis

#define perform_pag_analysis(analysis) xperform(analysis)
#define xperform(analysis) perform_pag_analysis_##analysis

#define carrier_type_o(type) xcarrier_type(type)
#define xcarrier_type(type) o_##type

#define carrier_printfunc(type) xcarrier_printfunc(type)
#define xcarrier_printfunc(type) o_##type##_print

typedef std::vector<std::pair<SgNode*,SgNode*>*> ExpressionPairVector;

DFI_STORE perform_pag_analysis(ANALYSIS)(SgProject* root,char* output,bool noresult);
std::string get_statement_pre_info_string(DFI_STORE store, SgStatement* stmt);
std::string get_statement_post_info_string(DFI_STORE store, SgStatement* stmt);
carrier_type_o(CARRIER_TYPE) get_statement_pre_info(DFI_STORE store, SgStatement *stmt);
carrier_type_o(CARRIER_TYPE) get_statement_post_info(DFI_STORE store, SgStatement *stmt);
carrier_type_o(CARRIER_TYPE) select_info(DFI_STORE store, SgStatement *stmt, std::string attrName);

std::string get_statement_alias_pairs_string(std::string pos, std::string alias_type, SgStatement *stmt);

class AliasPairAttribute : public AstAttribute {
private: 
  ExpressionPairVector *pairs;
public:
  AliasPairAttribute(ExpressionPairVector *es) { pairs = es; }
  ExpressionPairVector *getPairs() {return pairs;}
};

/* adds AliasPairs to AST */
template<typename DFI_STORE_TYPE>
class AliasPairsAnnotator : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
private:
  ExpressionPairVector *candidate_pairs;
  
public:
  AliasPairsAnnotator(DFI_STORE store_, ExpressionPairVector *candidate_pairs_) : StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {
    candidate_pairs = candidate_pairs_;
  }
  virtual ~AliasPairsAnnotator() {}

protected:
  virtual void visitStatement(SgStatement* stmt) {
    addAliasPairsAttribute(statementPreMustAliases(stmt),   "pre", "must", stmt);
    addAliasPairsAttribute(statementPostMustAliases(stmt), "post", "must", stmt);
    addAliasPairsAttribute(statementPreMayAliases(stmt),    "pre", "may",  stmt);
    addAliasPairsAttribute(statementPostMayAliases(stmt),  "post", "may",  stmt);
  }

  ExpressionPairVector *statementPreMustAliases(SgStatement *stmt) {
    return computeMustAliasPairs(get_statement_pre_info(store,stmt));
  }
  ExpressionPairVector *statementPostMustAliases(SgStatement *stmt) {
    return computeMustAliasPairs(get_statement_post_info(store,stmt));
  }
  ExpressionPairVector *statementPreMayAliases(SgStatement *stmt) {
    return computeMayAliasPairs(get_statement_pre_info(store,stmt));
  }
  ExpressionPairVector *statementPostMayAliases(SgStatement *stmt) {
    return computeMayAliasPairs(get_statement_post_info(store,stmt));
  }

  ExpressionPairVector *computeMustAliasPairs(carrier_type_o(CARRIER_TYPE) sg) {
    ExpressionPairVector *aliases = new ExpressionPairVector();
    ExpressionPairVector::iterator i;
    std::pair<SgNode*,SgNode*> *pair;
    for (i=candidate_pairs->begin(); i!=candidate_pairs->end(); i++) {
      pair = *i;
      if (o_is_must_alias(pair->first, pair->second, sg)) {
        aliases->push_back(pair);
      }
    }
    return aliases;
  }
  
  ExpressionPairVector *computeMayAliasPairs(carrier_type_o(CARRIER_TYPE) sg) {
    ExpressionPairVector *aliases = new ExpressionPairVector();
    ExpressionPairVector::iterator i;
    std::pair<SgNode*,SgNode*> *pair;
    for (i=candidate_pairs->begin(); i!=candidate_pairs->end(); i++) {
      pair = *i;
      if (o_is_may_alias(pair->first, pair->second, sg)) {
        aliases->push_back(pair);
      }
    }
    return aliases;
  }

  void addAliasPairsAttribute(ExpressionPairVector *exprs, std::string pos, std::string alias_type, SgStatement* node) {
    std::string label = "PAG " + alias_type + "-alias " + pos;
    node->setAttribute(label, new AliasPairAttribute(exprs));
  }
};


/* prints carrier */
template <typename DFI_STORE_TYPE>
class PagDfiTextPrinter : public DfiTextPrinter<DFI_STORE_TYPE> {
public:
  PagDfiTextPrinter(DFI_STORE_TYPE store) : DfiTextPrinter<DFI_STORE>(store) {}
  std::string statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_pre_info_string(store, stmt);
  }
  std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_post_info_string(store, stmt);
  }
};

/* prints AliasPairs */
template <typename DFI_STORE_TYPE>
class AliasPairsTextPrinter : public DfiTextPrinter<DFI_STORE_TYPE> {
private:
    ExpressionPairVector *pairs;
public:
    AliasPairsTextPrinter(DFI_STORE_TYPE store, ExpressionPairVector *_pairs) : DfiTextPrinter<DFI_STORE>(store) {
        pairs = _pairs;
    }
  
protected:
    virtual void visitStatement(SgStatement* stmt) {
        std::string preInfo = getPreInfo(stmt);
        std::string postInfo = getPostInfo(stmt);
        std::string stmt_str = stmt->unparseToString();
        
        std::cout << currentFunction() << ": " << "// pre must_aliases : " << 
          get_statement_alias_pairs_string("pre", "must", stmt) << std::endl;
        
        std::cout << currentFunction() << ": " << "// pre may_aliases : " << 
          get_statement_alias_pairs_string("pre", "may", stmt) << std::endl;
        
        std::cout << currentFunction() << ": " << stmt_str << std::endl;
        
        std::cout << currentFunction() << ": " << "// post must_aliases : " << 
          get_statement_alias_pairs_string("post", "must", stmt) << std::endl;
        
        std::cout << currentFunction() << ": " << "// post may_aliases : " << 
          get_statement_alias_pairs_string("post", "may", stmt) << std::endl << std::endl;
    }
};

/* adds carrier as comment to sourcecode */
template <typename DFI_STORE_TYPE>
class PagDfiCommentAnnotator : public DfiCommentAnnotator<DFI_STORE_TYPE> {
public:
  PagDfiCommentAnnotator(DFI_STORE_TYPE store) : DfiCommentAnnotator<DFI_STORE>(store) {}
  std::string statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_pre_info_string(store, stmt);
  }
  std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_post_info_string(store, stmt);
  }
};

/* adds AliasPairs as comment to sourcecode */
template <typename DFI_STORE_TYPE>
class AliasPairsCommentAnnotator : public DfiCommentAnnotator<DFI_STORE_TYPE> {
private:
    ExpressionPairVector *pairs;
public:
    AliasPairsCommentAnnotator(DFI_STORE_TYPE store, ExpressionPairVector *_pairs) : DfiCommentAnnotator<DFI_STORE>(store) {
        pairs = _pairs;
    }
protected:
    virtual void visitStatement(SgStatement* stmt) {
        addCommentBeforeNode("// pre must_aliases : " + get_statement_alias_pairs_string("pre", "must", stmt), stmt);
        addCommentBeforeNode("// pre may_aliases : "  + get_statement_alias_pairs_string("pre", "may",  stmt), stmt);
        addCommentAfterNode("// post must_aliases : " + get_statement_alias_pairs_string("post","must", stmt), stmt);
        addCommentAfterNode("// post may_aliases : "  + get_statement_alias_pairs_string("post","may",  stmt), stmt);
    }
};


struct ltexpr {
    bool operator()(SgNode* a, SgNode* b) const {
        return a->unparseToString() < b->unparseToString();
    }
};

class ExpressionCollector {
public:
    ExpressionPairVector* getExpressionPairs(SgProject *projectNode) {


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
        ExpressionPairVector *pairs = new ExpressionPairVector();

        std::set<SgNode*,ltexpr>::iterator i,j;
        for (i=exprs->begin(); i!=exprs->end(); i++) {
            j=i;
            for (j++; j!=exprs->end(); j++) {
                SgNode *a,*b;
                a = *i;
                b = *j;
                pairs->push_back(new std::pair<SgNode*,SgNode*>(a,b));
            }
        }
        
        free(exprs);
        exprs = NULL;
        
        return pairs;
    }
};

#endif
