/** 
 * \file MDCG/include/MDCG/model-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_BUILDER_HPP__
#define __MDCG_MODEL_BUILDER_HPP__

#include "MDCG/model.hpp"

//#include "MDCG/handle-builder.hpp"

#include <map>
#include <vector>
#include <iostream>

namespace MFB {
  template <template <typename T> class Model> class Driver;

  template <typename Object> class Sage;

  struct api_t;
};

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

class ModelBuilder /*:
  public Handles::handle_hook_t<ModelBuilder, build_model_t>,
  public Handles::handle_hook_t<ModelBuilder, get_model_t>*/
{
  private:
    MFB::Driver<MFB::Sage> & p_mfb_driver;
    std::vector<Model::model_t> p_models;

  private:
    
    template <Model::model_elements_e kind>
    void toDotNode(std::ostream & out, Model::element_t<kind> * element) const;

    template <Model::model_elements_e kind>
    void setParentFromScope(Model::model_t & model, Model::element_t<kind> * element, SgSymbol * symbol);
  
    void add(Model::model_t & model, const MFB::api_t * api);
    void add(Model::model_t & model, SgNamespaceSymbol * namespace_symbol);
    void add(Model::model_t & model, SgVariableSymbol * variable_symbol);
    void add(Model::model_t & model, SgFunctionSymbol * function_symbol);
    void add(Model::model_t & model, SgClassSymbol * class_symbol);
    void add(Model::model_t & model, SgMemberFunctionSymbol * member_function_symbol);

    void add(Model::model_t & model, SgType * type);

  public:
    ModelBuilder(MFB::Driver<MFB::Sage> & mfb_driver);

    unsigned create();

    void addOne(
      unsigned model,
      const std::string & name,
      const std::string & path,
      std::string suffix
    );

    void addPair(
      unsigned model,
      const std::string & name,
      const std::string & header_path,
      const std::string & source_path,
      std::string header_suffix,
      std::string source_suffix
    );

    const Model::model_t & get(unsigned model) const;

    void print(std::ostream & out, unsigned model) const;
};

/** @} */

}

#endif /* __MDCG_MODEL_BUILDER_HPP__ */

