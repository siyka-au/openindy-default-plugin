#ifndef CFUTIL_H
#define CFUTIL_H

#include <string>

#include "types.h"

using namespace oi;

class CFUtil {
 public:
  CFUtil();

  static FeatureTypes featureTypesForName(std::string name);
};

#endif
