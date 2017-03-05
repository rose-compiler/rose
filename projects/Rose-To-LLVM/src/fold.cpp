#include <rose.h>
#include <rosetollvm/ConstantFolder.h>

#include <iostream>
#include <vector>

class ForestPruner : public AstPrePostProcessing {
  public:
    void prune(SgProject *project) {
      AstPrePostProcessing::traverse(project);
      for(
        std::list<
          std::pair<SgExpression*, SgExpression*>
        >::const_iterator
          itr = replacements.begin(), end = replacements.end();
        itr != end; ++itr
      ) {
        SageInterface::replaceExpression(itr->first, itr->second);
      }
      replacements.clear();
    }
  private:
    void preOrderVisit(SgNode *n) {}
    void postOrderVisit(SgNode *n) {
      if (SgValueExp *val_exp = isSgValueExp(n)) {
        SgExpression *oet = val_exp->get_originalExpressionTree();
        if (oet) {
          val_exp->set_originalExpressionTree(NULL);
          replacements.push_back(std::make_pair(val_exp, oet));
        }
      }
      else if (SgCastExp *cast_exp = isSgCastExp(n)) {
        SgExpression *oet = cast_exp->get_originalExpressionTree();
        SgExpression *operand = cast_exp->get_operand();
        if (oet) {
          cast_exp->set_originalExpressionTree(NULL);
          replacements.push_back(std::make_pair(operand, oet));
        }
      }
    }
    std::list<std::pair<SgExpression*, SgExpression*> > replacements;
    // Unimplemented.
    void traverse(SgNode*);
    void traverseWithinFile(SgNode*);
    void traverseInputFiles(SgProject*);
};

class FindAndFold : public AstPrePostProcessing {
  public:
    FindAndFold() : scopeName("<global>"), reportedScope(false) {}
    void find(SgProject *project) {
      AstPrePostProcessing::traverse(project);
    }
  private:
    void preOrderVisit(SgNode *n) {
      if (isSgFunctionDeclaration(n)) {
        scopeName = isSgFunctionDeclaration(n)->get_name().getString();
      }
      else if (isSgInitializedName(n)) {
        SgInitializedName *var = isSgInitializedName(n);
        std::string var_name = var->get_name().getString();
        if (var_name.substr(0, 4) == "fold") {
          if (!reportedScope) {
            std::cout << scopeName << ":" << std::endl;
            reportedScope = true;
          }
          SgAssignInitializer *init =
            isSgAssignInitializer(var->get_initializer());
          if (init) {
            SgExpression *expr = init->get_operand();
            SgExpression *fold = folder.fold(expr);
            std::cout
              << "  " << var_name << ":" << std::endl
              << "    expr: " << expr->unparseToString() << std::endl
              << "    fold: " << fold->unparseToString() << std::endl;
            SageInterface::deepDelete(fold);
          }
        }
      }
    }
    void postOrderVisit(SgNode* n) {
      if (isSgFunctionDeclaration(n)) {
        scopeName = "<global>";
        reportedScope = false;
      }
    }
    ConstantFolder::ExpressionTraversal folder;
    std::string scopeName;
    bool reportedScope;
    // Unimplemented.
    void traverse(SgNode*);
    void traverseWithinFile(SgNode*);
    void traverseInputFiles(SgProject*);
};

int
main(int argc, char *argv[])
{
  SgStringList args =
    CommandlineProcessing::generateArgListFromArgcArgv (argc, argv);
  SgProject *project = frontend(args);
  generateDOT(*project, "");
  ForestPruner pruner;
  pruner.prune(project);
  generateDOT(*project, "-pruned");
  FindAndFold finder;
  finder.find(project);
  SageInterface::deepDelete(project);
  return 0;
}
