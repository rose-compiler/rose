
#ifndef __KLT_API_HPP__
#define __KLT_API_HPP__

class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgType;
class SgInitializedName;
class SgExpression;
class SgStatement;
class SgScopeStatement;
namespace MDCG {
  namespace Model {
    struct model_t;
  }
}

namespace KLT {

namespace API {

struct kernel_t {
  private:
    SgClassSymbol * klt_loop_context_class;

    SgFunctionSymbol * get_loop_lower_fnct;
    SgFunctionSymbol * get_loop_upper_fnct;
    SgFunctionSymbol * get_loop_stride_fnct;

    SgFunctionSymbol * get_tile_length_fnct;
    SgFunctionSymbol * get_tile_stride_fnct;

    virtual void load_user(const MDCG::Model::model_t & model);

  public:
    kernel_t();
    ~kernel_t();

    SgType * addContextTypeModifier(SgType * type) const;

    SgInitializedName * createContext() const;

    SgExpression * buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const;

    SgExpression * buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const;

    SgClassSymbol * getLoopContextClass() const;

    void load(const MDCG::Model::model_t & model);
};

struct host_t {
  private:
    SgClassSymbol * kernel_class;
    SgClassSymbol * loop_class;
    SgClassSymbol * tile_class;
    SgClassSymbol * data_class;

    SgVariableSymbol * kernel_param_field;
    SgVariableSymbol * kernel_data_field;
    SgVariableSymbol * data_ptr_field;
    SgVariableSymbol * data_section_field;
    SgVariableSymbol * section_offset_field;
    SgVariableSymbol * section_length_field;

    SgVariableSymbol * kernel_loop_field;
    SgVariableSymbol * loop_lower_field;
    SgVariableSymbol * loop_upper_field;
    SgVariableSymbol * loop_stride_field;

    SgFunctionSymbol * build_kernel_func;
    SgFunctionSymbol * execute_kernel_func;

    virtual void load_user(const MDCG::Model::model_t & model);

  public:
    host_t();
    ~host_t();

    SgVariableSymbol * insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const;
    void insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const;

    SgStatement * buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;

    SgStatement * buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const;
    SgStatement * buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const;

    SgStatement * buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;

    SgClassSymbol * getKernelClass() const;
    SgClassSymbol * getLoopClass() const;
    SgClassSymbol * getTileClass() const;
    SgClassSymbol * getDataClass() const;

    void load(const MDCG::Model::model_t & model);
};

} // namespace KLT::API

} // namespace KLT

#endif /* __KLT_API_HPP__ */

