#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_SYSTEM_TRANSFORMATION_P_HELMERT6_PARAM_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_SYSTEM_TRANSFORMATION_P_HELMERT6_PARAM_H_

#include "p_helmert7_param.h"

using namespace oi;

/*!
 * \brief The Helmert6Param class is a helmert transformation without scale
 * (scale fixed to 1). Reuses Helmert7Param's math by restricting its
 * "calculate scale" parameter to "no", which routes Helmert7Param::exec()
 * to its existing unscaled 6-parameter (calc_6p) path.
 */
class Helmert6Param : public Helmert7Param {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_SYSTEM_TRANSFORMATION_P_HELMERT6_PARAM_H_
