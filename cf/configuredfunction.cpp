#include "configuredfunction.h"

#include <QtCore>
#include <vector>

#include "cfvisitor.h"
#include "treeutil.h"
#include "types.h"

ConfiguredFunction::ConfiguredFunction(
    ConfiguredFunctionConfig config,
    std::vector<oi::Guarded<Function> > functions)
    : SpecialFunction(), functions(functions), config(config) {}

void ConfiguredFunction::init() {
  this->metaData.name = this->config.name;
  this->metaData.iid = SpecialFunction_iidd;  // or differentiate?
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "";
  this->metaData.description = this->config.description;

  this->applicableFor = this->config.applicableFor;
  this->neededElements = config.getNeededElements();
}

std::expected<void, std::string> ConfiguredFunction::execChecked(
    const oi::Guarded<FeatureWrapper>& feature,
    const oi::OperationContext& operation) {
  try {
    ListVisitor visitors;

    QPointer<NodeVisitor> debug = new PrintVisitor();
    visitors.list.push_back(debug);

    CFContext ctx;  // contains references not copies !
    ctx.config = this->config;
    ctx.operation = operation;  // the visitor polls it between functions
    ctx.baseFunction = this;
    ctx.baseFeature = feature;
    ctx.functions = this->functions;  // all necessary funktions

    for (auto function : ctx.functions) {
      function->clear();  // remove inner data e.g. inputElements
    }

    ctx.global_inputElements = this->global_inputElements.empty()
                                   ? this->getInputElements()
                                   : this->global_inputElements;
    ctx.global_feature =
        this->global_feature.isNull() ? feature : this->global_feature;

    QPointer<NodeVisitor> cf;
    cf = new CFVisitor(ctx);

    visitors.list.push_back(cf);

    TreeUtil::traversal(this->config.parameter, visitors);

    return {};
  } catch (exception& e) {
    sendMessage(e.what(), MessageTypes::eErrorMessage,
                MessageDestinations::eConsoleMessage);
    return std::unexpected(std::string(e.what()));
  }
}

std::string ConfiguredFunction::prettyPrint() {
  std::string s;
  s += "function: '" + this->getMetaData().name + "', featureFunctions: [ ";
  for (oi::Guarded<Function> f : this->functions) {
    if (auto* configured = dynamic_cast<ConfiguredFunction*>(f.data())) {
      s += "'" + configured->prettyPrint() + "', ";
    } else {
      s += "'" + f->getMetaData().name + "', ";
    }
  }
  s += " ], ";
  return s;
}
