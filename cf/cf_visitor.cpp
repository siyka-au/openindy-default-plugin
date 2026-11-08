#include "cf_visitor.h"

#include <QtCore>
#include <format>
#include <stdexcept>
#include <utility>

#include "cf_function_data.h"
#include "cf_util.h"
#include "cfc_parameter.h"
#include "configured_function.h"
#include "configured_function_config.h"

CFVisitor::CFVisitor(CFContext ctx) : ctx(std::move(ctx)) {}

void CFVisitor::pre(QPointer<Node> n, int index, int level) {
  CFCParameter* node = static_cast<CFCParameter*>(n.data());

  std::string pre = std::format("pre L{} I{}", level, index);

  if (level == 0) {  // level 0, first level

    CFFunctionData fd;
    fd.function = this->ctx.baseFunction;
    fd.feature = this->ctx.baseFeature;  // == ctx.global_feature
    this->data.push(fd);

  } else if (ctx.config.isFunction(
                 node->getName())) {  // level 1-n, all other levels

    oi::Guarded<Function> parentFunction = data.top().function;

    CFFunctionData fd;
    fd.function = this->ctx.getFunction(node->getName());

    // set feature for function
    if (node->getFeature().compare("global") == 0) {
      fd.feature = ctx.global_feature;
      debug(pre,
            std::format("set global feature: {1} to function: {0}",
                        fd.function->getMetaData().name,
                        fd.feature->getFeature()->getFeatureName()),
            level);
    } else if (node->getFeature().compare("global.copy") == 0) {
      oi::Guarded<FeatureWrapper> copy;
      switch (ctx.global_feature->getFeatureTypeEnum()) {
        case FeatureTypes::ePointFeature: {
          oi::Guarded<Point> src = ctx.global_feature->getPoint();
          copy = ctx.adopt(std::make_unique<Point>(src->getPosition()));
          break;
        }
        default:
          throw std::logic_error(
              std::format("no FeatureTypes for \"{}\" found",
                          ctx.global_feature->getFeatureTypeString()));
          break;
      }
      fd.feature = copy;

      debug(pre,
            std::format("set global.copy feature: {1} to function: {0}",
                        fd.function->getMetaData().name,
                        fd.feature->getFeature()->getFeatureName()),
            level);
    } else {
      oi::Guarded<FeatureWrapper> feature = this->ctx.createApplicableFeature(
          CFUtil::featureTypesForName(node->getFeature()));
      fd.feature = feature;
      debug(pre,
            std::format("set feature: {1} to function: {0}",
                        fd.function->getMetaData().name,
                        fd.feature->getFeature()->getFeatureName()),
            level);
    }

    // add input elements to parent, but not for level 1 or lower, because that
    // overwrites you base input elements
    if (level > 1) {
      InputElement ie = ctx.createInputElement(fd.feature);

      debug(pre,
            std::format(
                "add input element id: {1}, {2}, {3} to function: {0} ({4})",
                parentFunction->getMetaData().name, ie.id,
                static_cast<int>(ie.typeOfElement), ie.label, index),
            level);
      parentFunction->addInputElement(ie, index);
    }

    this->data.push(fd);

  } else if (ctx.config.isNeededElement(node->getName())) {
    CFFunctionData parentFd = data.top();
    oi::Guarded<Function> parentFunction = parentFd.function;

    InputElement ie = ctx.getInputElement(node->getName());

    debug(pre,
          std::format(
              "add input element id: {1}, {2}, {3} to function: {0} ({4})",
              parentFunction->getMetaData().name, ie.id,
              static_cast<int>(ie.typeOfElement), ie.label, index),
          level);
    parentFunction->addInputElement(ie, index);
  }
}

void CFVisitor::post(QPointer<Node> n, int index, int level) {
  CFCParameter* node = static_cast<CFCParameter*>(n.data());

  std::string post = std::format("post L{} I{}", level, index);

  if (ctx.config.isFunction(node->getName()) &&
      level > 0 /* skip level 0 because this is the base function */) {
    CFFunctionData fd = data.pop();

    ConfiguredFunction* cf = dynamic_cast<ConfiguredFunction*>(
        static_cast<oi::Function*>(fd.function));  // inner ConfiguredFunction
    if (cf) {
      cf->setGlobalInputElements(ctx.global_inputElements);
      // maybe later there is a need for "global.global"
      // cf->global_feature = ctx.global_feature;
    }

    debug(post, std::format("exec featureData: {}", fd.prettyPrint()), level);
    ctx.operation.throwIfCancelled();
    if (!fd.function->exec(fd.feature)) {
      throw std::logic_error(
          std::format("execution failed for function: '{}'' with feature: '{}'",
                      fd.function->getMetaData().name,
                      fd.feature->getFeature()->getFeatureName()));
    }
  }
}
