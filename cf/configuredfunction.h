#ifndef CONFIGUREDFUNCTION_H
#define CONFIGUREDFUNCTION_H
#include <QObject>
#include <map>
#include <vector>

#include "configuredfunctionconfig.h"
#include "featurewrapper.h"
#include "function.h"
#include "specialfunction.h"

using namespace oi;

/**
 * @brief The ConfiguredFunction class
 */
class ConfiguredFunction : public SpecialFunction {
  friend class Feature;
  friend class CFVisitor;

 public:
  ConfiguredFunction(ConfiguredFunctionConfig config,
                     std::vector<oi::Guarded<Function> > functions);
  void init() override;

  // Stage 7d/D21: overrides execChecked(), not exec(), because
  // Feature::recalc() calls execChecked() directly (see feature.cpp) -
  // the inherited bool Function::exec() wrapper forwards to whichever
  // execChecked() override is most-derived, so cfvisitor.cpp's
  // fd.function->exec(fd.feature) (called through a base Function*) keeps
  // working unchanged. A ConfiguredFunction manages its own sub-function
  // traversal (CFVisitor) rather than going through
  // Function::execByFeatureType's Producer<T>/Transform<T> dispatch -
  // Function::canActAsProducer/canActAsTransform exempt any
  // SpecialFunction from the mixin check for the same reason.
  std::expected<void, QString> execChecked(
      const oi::Guarded<FeatureWrapper>& feature) override;

  QString prettyPrint();

 public:
  std::map<int, std::vector<InputElement> > global_inputElements;
  oi::Guarded<FeatureWrapper> global_feature;

 private:
  ConfiguredFunctionConfig config;
  // all necessary funktions
  std::vector<oi::Guarded<Function> > functions;
};

#endif  // CONFIGUREDFUNCTION_H
