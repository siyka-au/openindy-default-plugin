#ifndef OI_DEFAULT_PLUGIN_CF_CF_UTIL_H_
#define OI_DEFAULT_PLUGIN_CF_CF_UTIL_H_

#include <string>

#include "types.h"

using namespace oi;

class CFUtil {
 public:
  CFUtil();

  static FeatureTypes featureTypesForName(std::string name);
};

#endif  // OI_DEFAULT_PLUGIN_CF_CF_UTIL_H_
