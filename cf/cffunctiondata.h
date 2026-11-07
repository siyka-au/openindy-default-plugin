#ifndef CFFUNCTIONDATA_H
#define CFFUNCTIONDATA_H

#include <QString>

#include "featurewrapper.h"
#include "function.h"

using namespace oi;

class CFFunctionData {
 public:
  CFFunctionData();

  QString prettyPrint();

 public:
  oi::Guarded<Function> function;

  // feature returned / modified by this function
  oi::Guarded<FeatureWrapper> feature;
};

#endif  // CFFUNCTIONDATA_H
