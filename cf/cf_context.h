#ifndef OI_DEFAULT_PLUGIN_CF_CF_CONTEXT_H_
#define OI_DEFAULT_PLUGIN_CF_CF_CONTEXT_H_
#include <QPointer>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "configured_function.h"
#include "feature_wrapper.h"
#include "function.h"
#include "types.h"

using namespace oi;

class CFContext {
 public:
  CFContext();

  // progress and cancellation of the exec this context belongs to
  oi::OperationContext operation;

  /**
   * @brief getFunction
   * @param name
   * @return function instance and removes function from list
   */
  oi::Guarded<Function> getFunction(std::string name);

  InputElement getInputElement(std::string name);

  /**
   * @brief createApplicableFeature
   * @param featureType
   * @return feature for element of featuresTypes
   */
  oi::Guarded<FeatureWrapper> createApplicableFeature(FeatureTypes featureType);

  // takes ownership of a generated scratch feature, returns its wrapper
  oi::Guarded<FeatureWrapper> adopt(std::unique_ptr<Feature> feature);
  InputElement createInputElement(oi::Guarded<FeatureWrapper> feature);

 public:
  ConfiguredFunctionConfig config;
  std::map<int, std::vector<InputElement>> inputElements;
  // all necessary funktions
  std::vector<oi::Guarded<Function>> functions;

  oi::Guarded<Function> baseFunction;
  oi::Guarded<FeatureWrapper> baseFeature;

  std::map<int, std::vector<InputElement>> global_inputElements;
  oi::Guarded<FeatureWrapper> global_feature;
  // the generated scratch features live as long as the context
  std::vector<std::unique_ptr<Feature>> generatedFeatures;
};

#endif  // OI_DEFAULT_PLUGIN_CF_CF_CONTEXT_H_
