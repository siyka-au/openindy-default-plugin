#ifndef OI_DEFAULT_PLUGIN_CF_CF_FUNCTION_DATA_H_
#define OI_DEFAULT_PLUGIN_CF_CF_FUNCTION_DATA_H_

#include <string>

#include "feature_wrapper.h"
#include "function.h"

using namespace oi;

class CFFunctionData {
 public:
  CFFunctionData();

  std::string prettyPrint();

 public:
  oi::Guarded<Function> function;

  // feature returned / modified by this function
  oi::Guarded<FeatureWrapper> feature;
};

#endif  // OI_DEFAULT_PLUGIN_CF_CF_FUNCTION_DATA_H_
