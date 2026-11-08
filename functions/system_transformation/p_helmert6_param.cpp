#include "p_helmert6_param.h"

#include <format>

/*!
 * \brief Helmert6Param::init
 */
void Helmert6Param::init() {
  Helmert7Param::init();

  // set plugin meta data
  this->metaData.name = "Helmert6Param";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra, jwa";
  this->metaData.description = std::format(
      "{}",
      "This function calculates a helmert transformation without scale (scale "
      "fixed to 1).");
  this->metaData.iid = SystemTransformation_iidd;

  // restrict to the no-scale path - Helmert7Param::exec() already calls
  // calc_6p() when this resolves to "no"
  this->stringParameters.erase("calculate scale");
  this->stringParameters.emplace("calculate scale", "no");

  // drop scale-only parameters that no longer apply
  this->stringParameters.erase("use temperature");
  this->stringParameters.erase("use reference temperature");
  this->stringParameters.erase("material");
  this->doubleParameters.erase("reference");
  this->doubleParameters.erase("actual");
}
