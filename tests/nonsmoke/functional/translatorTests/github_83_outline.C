//From https://raw.githubusercontent.com/forflo/bugReproductionPackage05/master/outline.cpp

#include <rose.h>
#include <stdio.h>
#include <Outliner.hh>

using namespace SageInterface;

// Wrapper around Rose's Outliner to emulate statelessness
class OutlWrap {
  bool dumpOutlinedCodeIntoNewFile = false;
  bool enableClassicOutliner = true;

public:
  OutlWrap(bool newFile, bool enableClassic)
      : dumpOutlinedCodeIntoNewFile(newFile),
        enableClassicOutliner(enableClassic) {}

  Outliner::Result outline(SgStatement *s) const {
    Outliner::enable_classic = enableClassicOutliner;
    Outliner::useNewFile = dumpOutlinedCodeIntoNewFile;
    // Outliner::naturalScopeHandling = true;
    return Outliner::outline(s);
  }
};

template <typename T> class PostOrderSequencer {
private:
  class Traverser : public AstSimpleProcessing {
  public:
    std::vector<T *> sequence{};
    virtual void atTraversalEnd() {}
    virtual void visit(SgNode *node) {
      auto decl = dynamic_cast<T *>(node);
      if (!decl) {
        return;
      }
      sequence.push_back(decl);
    }
  };

public:
  static std::vector<T *> sequence(SgNode *ast) {
    Traverser traverser{};
    traverser.traverse(ast, postorder);
    return traverser.sequence;
  }
};

int main(int argc, char **argv) {
  SgProject *project = frontend(argc, argv);

  // query the omp target node ...
  std::vector<SgOmpTargetDataStatement *> targetRegions =
      PostOrderSequencer<SgOmpTargetDataStatement>::sequence(project);
  ROSE_ASSERT(targetRegions.size() == 1);
  SgOmpTargetDataStatement *targetRegion = targetRegions[0];

  ROSE_ASSERT(targetRegion != NULL);

  SgFunctionDeclaration *main = SageInterface::findMain(project);
  SgSourceFile *mainFile = SageInterface::getEnclosingSourceFile(main);

  ROSE_ASSERT(main != NULL);

  // ... get the SgBasicBlock beneath the omp target node ...
  SgStatement *body = targetRegion->get_body();
  ROSE_ASSERT(body->variantT() == V_SgBasicBlock);


  OutlWrap outliner(false, true);
  auto outlinerResult = outliner.outline(body);
  SgFunctionDeclaration *outlinedFunction = outlinerResult.decl_;

  ROSE_ASSERT(outlinedFunction != NULL);
  backend(project);

  return 0;
}
