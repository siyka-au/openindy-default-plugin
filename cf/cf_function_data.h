#ifndef CFFUNCTIONDATA_H
#define CFFUNCTIONDATA_H

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

#endif  // CFFUNCTIONDATA_H
