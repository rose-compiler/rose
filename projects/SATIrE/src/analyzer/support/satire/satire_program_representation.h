#ifndef H_SATIRE_PROGRAM_REPRESENTATION
#define H_SATIRE_PROGRAM_REPRESENTATION

#include "satire.h"
#include "aralir.h"

namespace SATIrE
{

// This class provides a unified program representation abstraction. When
// constructed from a command line, it will build an AST from the input
// files. The corresponding ICFG representation is not built automatically;
// the intention is to have it built by the first analysis that needs it.
// These representations are owned by the Program instance.
class Program
{
public:
    AnalyzerOptions *options;
    SgProject *astRoot;
    CFG *icfg;
    PrologTerm *prologTerm;

    Program(AnalyzerOptions *o);
    ~Program();

 // Our own symbol tables for file-scope variables, because ROSE's are
 // broken.
    typedef std::map<std::string,
                     std::pair<SgVariableSymbol *, SgInitializedName *> >
            global_map_t;
    typedef std::map<std::pair<std::string, SgFile *>,
                     std::pair<SgVariableSymbol *, SgInitializedName *> >
            static_map_t;
    global_map_t global_map;
    static_map_t static_map;

    SgVariableSymbol *get_symbol(SgInitializedName *initName);
    SgVariableSymbol *get_symbol(SgVarRefExp *varRef);

private:
 // No copying.
    Program(const Program &);
    const Program & operator=(const Program &);

    void fixVariableSymbols(void);
};

}

// MS: 2009
class AstAralAttribute: public AstAttribute {
  public:
    void setInfoElementList(Aral::InfoElementList* ieList) {
      _infoElementList=ieList;
    }
    Aral::InfoElementList* getInfoElementList() {
      return _infoElementList;
    }
  private:
    Aral::InfoElementList* _infoElementList;
};


#endif
