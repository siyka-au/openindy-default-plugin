#ifndef OI_DEFAULT_PLUGIN_CF_CONFIGURED_FUNCTION_H_
#define OI_DEFAULT_PLUGIN_CF_CONFIGURED_FUNCTION_H_
#include <QObject>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "configured_function_config.h"
#include "feature_wrapper.h"
#include "function.h"
#include "special_function.h"

using namespace oi;

/**
 * @brief The ConfiguredFunction class
 */
class ConfiguredFunction : public SpecialFunction {
  friend class Feature;
  friend class CFVisitor;

 public:
  ConfiguredFunction(ConfiguredFunctionConfig config,
                     std::vector<std::unique_ptr<Function>> functions);
  void init() override;

  // Stage 7d/D21: overrides execChecked(), not exec(), because
  // Feature::recalc() calls execChecked() directly (see feature.cpp) -
  // the inherited bool Function::exec() wrapper forwards to whichever
  // execChecked() override is most-derived, so cf_visitor.cpp's
  // fd.function->exec(fd.feature) (called through a base Function*) keeps
  // working unchanged. A ConfiguredFunction manages its own sub-function
  // traversal (CFVisitor) rather than going through
  // Function::execByFeatureType's Producer<T>/Transform<T> dispatch -
  // Function::canActAsProducer/canActAsTransform exempt any
  // SpecialFunction from the mixin check for the same reason.
  std::expected<void, std::string> execChecked(
      const oi::Guarded<FeatureWrapper>& feature,
      const oi::OperationContext& operation = {}) override;

  std::string prettyPrint();

 public:
  // inputs inherited from the enclosing configured function
  void setGlobalInputElements(
      std::map<int, std::vector<InputElement>> elements) {
    global_inputElements = std::move(elements);
  }

 private:
  std::map<int, std::vector<InputElement>> global_inputElements;
  oi::Guarded<FeatureWrapper> global_feature;
  ConfiguredFunctionConfig config;
  // all necessary funktions: owned here, viewed through `functions`
  std::vector<std::unique_ptr<Function>> ownedFunctions;
  std::vector<oi::Guarded<Function>> functions;
};

#endif  // OI_DEFAULT_PLUGIN_CF_CONFIGURED_FUNCTION_H_
