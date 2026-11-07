#ifndef CFCONTEXT_H
#define CFCONTEXT_H

#include <QList>
#include <QMultiMap>
#include <QPointer>
#include <QString>

#include "configuredfunction.h"
#include "featurewrapper.h"
#include "function.h"
#include "types.h"

using namespace oi;

class CFContext {
 public:
  CFContext();

  /**
   * @brief getFunction
   * @param name
   * @return function instance and removes function from list
   */
  oi::Guarded<Function> getFunction(QString name);

  InputElement getInputElement(QString name);

  /**
   * @brief createApplicableFeature
   * @param featureType
   * @return feature for element of featuresTypes
   */
  oi::Guarded<FeatureWrapper> createApplicableFeature(FeatureTypes featureType);
  InputElement createInputElement(oi::Guarded<FeatureWrapper> feature);

 public:
  ConfiguredFunctionConfig config;
  QMap<int, QList<InputElement> > inputElements;
  // all necessary funktions
  QList<oi::Guarded<Function> > functions;

  oi::Guarded<Function> baseFunction;
  oi::Guarded<FeatureWrapper> baseFeature;

  QMap<int, QList<InputElement> > global_inputElements;
  oi::Guarded<FeatureWrapper> global_feature;
};

#endif  // CFCONTEXT_H
