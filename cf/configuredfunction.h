#ifndef CONFIGUREDFUNCTION_H
#define CONFIGUREDFUNCTION_H

#include <QObject>

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

  Q_OBJECT

 public:
  ConfiguredFunction(ConfiguredFunctionConfig config,
                     QList<QPointer<Function> > functions,
                     QObject* parent = nullptr);
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
      const QPointer<FeatureWrapper>& feature) override;

  QString prettyPrint();

 public:
  QMap<int, QList<InputElement> > global_inputElements;
  QPointer<FeatureWrapper> global_feature;

 private:
  ConfiguredFunctionConfig config;
  // all necessary funktions
  QList<QPointer<Function> > functions;
};

#endif  // CONFIGUREDFUNCTION_H
