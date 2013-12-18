/** 
 * \file MDCG/lib/model-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MDCG/model-builder.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/Sage/driver.hpp"

#include <cassert>

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

ModelBuilder::ModelBuilder(MFB::Driver<MFB::Sage> & mfb_driver) :
  p_mfb_driver(mfb_driver),
  p_models()
{}

void ModelBuilder::add(unsigned model, const MFB::api_t * api) {
  assert(false); /// \todo
}
 
unsigned ModelBuilder::create() {
  unsigned model = p_models.size();
  p_models.push_back(Model::model_t());
  return model;
}

void ModelBuilder::addOne(
  unsigned model,
  const std::string & name,
  const std::string & path,
  std::string suffix
) {
  unsigned long file_id = p_mfb_driver.loadStandaloneSourceFile(name, path, suffix);

  MFB::api_t * api = p_mfb_driver.getAPI(file_id);

  add(model, api);
}

void ModelBuilder::addPair(
  unsigned model,
  const std::string & name,
  const std::string & header_path,
  const std::string & source_path,
  std::string header_suffix,
  std::string source_suffix
) {
  assert(false); /// \todo
}
 
const Model::model_t & ModelBuilder::get(const unsigned model_id) const {
  assert(false); /// \todo
}

void ModelBuilder::print(std::ostream & out, unsigned model) const {
  assert(false); /// \todo
}

/** @} */

}

