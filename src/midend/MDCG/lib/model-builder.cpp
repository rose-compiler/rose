/** 
 * \file MDCG/lib/model-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MDCG/model-builder.hpp"
#include "MFB/Sage/api.hpp"

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

ModelBuilder::ModelBuilder(MFB::ApiBuilder & api_builder) :
  p_api_builder(api_builder),
  p_models()
{}

const MFB::api_t & ModelBuilder::getApi(const std::string & file) const {
  /// \todo
}

Model::model_id_t ModelBuilder::add(Model::model_id_t model, const MFB::api_t & api) {
  /// \todo
}
 
Model::model_id_t ModelBuilder::build(const std::vector<std::string> & files) {
  /// \todo
}

Model::model_id_t ModelBuilder::add(Model::model_id_t model, std::vector<std::pair<std::string, bool> > & files) {
  /// \todo
}
 
const Model::Model & ModelBuilder::get(const Model::model_id_t model_id) const {
  /// \todo
}

/** @} */

}

