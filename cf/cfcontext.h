#ifndef CFCONTEXT_H
#define CFCONTEXT_H
#include <QPointer>
#include <QString>
#include <map>
#include <vector>

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
  std::map<int, std::vector<InputElement> > inputElements;
  // all necessary funktions
  std::vector<oi::Guarded<Function> > functions;

  oi::Guarded<Function> baseFunction;
  oi::Guarded<FeatureWrapper> baseFeature;

  std::map<int, std::vector<InputElement> > global_inputElements;
  oi::Guarded<FeatureWrapper> global_feature;
};

#endif  // CFCONTEXT_H
