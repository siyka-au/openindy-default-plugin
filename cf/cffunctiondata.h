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
  QPointer<Function> function;

  // feature returned / modified by this function
  QPointer<FeatureWrapper> feature;
};

#endif  // CFFUNCTIONDATA_H
