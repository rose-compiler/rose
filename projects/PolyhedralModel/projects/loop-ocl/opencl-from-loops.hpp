
#ifndef __OPENCL_FROM_LOOPS_HPP__
#define __OPENCL_FROM_LOOPS_HPP__

#include <vector>
#include <map>
#include <string>

class SgStatement;
class SgForStatement;
class SgVariableSymbol;
class SgSourceFile;
class SgProject;
class SgInitializedName;
class SgType;
class SgVarRefExp;
class SgExpression;

enum AccessType {
  in,
  out,
  inout,
  unknown
};

struct Scalar {
  Scalar(std::string name_);

  std::string name;

  SgType * type;

  AccessType access_type;
};

struct Array {
  Array(std::string name_);

  std::string name;

  SgType * base_type;

  std::vector<unsigned> dimensions;

  AccessType access_type;

  SgVariableSymbol * associated_ocl_buffer;
};

struct AnalysisContainer {
  AnalysisContainer();

  bool polyhedral_analysis_success;
  std::map<SgInitializedName *, Scalar> scalars;
  std::map<SgInitializedName *, Array>  arrays;

  SgFunctionDeclaration * base_func;
  std::vector<PolyhedricDependency::FunctionDependency *> * dependencies;
};

void findVariableType(SgInitializedName * init_name, AnalysisContainer & analysis);
void findVariableAccessType(std::vector<SgVarRefExp *> & var_refs, AnalysisContainer & analysis);

class Kernel {
  protected:
    unsigned id;
    unsigned nbr_work_dim;
    SgStatement * kernel_content;
    std::map<SgVariableSymbol *, std::pair<int, int> > iterators_bounds;
    std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > iterators_bounds_expression;
    std::vector<SgVariableSymbol *> ordered_iterators;
    std::string kernel_name;
    AnalysisContainer & analysis;

  public:
    Kernel(
            SgStatement * stmt,
            unsigned nbr_work_dim_,
            std::map<SgVariableSymbol *, std::pair<int, int> > iterators_bounds_,
            std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > iterators_bounds_expression_,
            std::vector<SgVariableSymbol *> ordered_iterators_,
            AnalysisContainer & analysis_
    );

    SgStatement * genCall();

    void createKernel(SgStatement ** previous, SgSourceFile * ocl_file);

  static unsigned id_cnt;
};

void GenerateKernel(
                     SgStatement * first,             
                     SgStatement * last,
                     unsigned dim_out,
                     unsigned dim_used,
                     std::vector<Kernel *> & generated_kernel,
                     AnalysisContainer & analysis
);

void createOclInit(SgStatement ** previous, std::string ocl_file_name, AnalysisContainer & analysis);

void createOclRelease(SgStatement ** previous, AnalysisContainer & analysis);

void genKernel(
                SgStatement * stmt,
                unsigned dim_used,
                std::vector<SgForStatement *> for_list,
                std::vector<Kernel *> & generated_kernel,
                std::map<SgVariableSymbol *, std::pair<int, int> > iterators_bounds,
                std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > iterators_bounds_expression,
                std::vector<SgVariableSymbol *> ordered_iterator,
                AnalysisContainer & analysis
);

SgStatement * traverseLeftOutLoop(
                             SgStatement * stmt,
                             unsigned dim_out,
                             unsigned dim_used,
                             std::vector<SgForStatement *> for_list,
                             std::vector<Kernel *> & generated_kernel_,
                             AnalysisContainer & analysis
);

SgSourceFile * buildOpenCLfile(std::string file_name, SgProject * project);

#endif /* __OPENCL_FROM_LOOPS_HPP__ */

