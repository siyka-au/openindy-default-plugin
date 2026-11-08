#ifndef FUNCTIONCONFIGPARSER_H
#define FUNCTIONCONFIGPARSER_H
#include <QtCore>
#include <map>
#include <string>
#include <vector>

#include "cf_context.h"
#include "cfc_parameter.h"
#include "configured_function_config.h"
#include "construct_function.h"
#include "feature_wrapper.h"
#include "tree_util.h"
#include "types.h"

using namespace oi;

/**
 * @brief parses JSON config file and generate list of FunctionConfigParser
 */
class FunctionConfigParser {
 public:
  std::vector<ConfiguredFunctionConfig> readConfigFromJson(
      std::string filename);
  std::vector<ConfiguredFunctionConfig> readConfigFromJson();

 private:
  std::vector<QPointer<Node> > parameter(QJsonValue value);

  std::multimap<int, InputElementMapping> inputElementsMapping(
      ConfiguredFunctionConfig config, QJsonValue value);

  std::vector<std::string> functionNames(QJsonValue value);

  ElementTypes typeOfElement(QJsonValue value);

  std::vector<std::pair<std::string, NeededElement> > neededElements(
      QJsonValue value);

  std::vector<FeatureTypes> applicableFor(QJsonValue value);

  std::vector<ConfiguredFunctionConfig> readConfigFromJson(QJsonObject json);
};

#endif  // FUNCTIONCONFIGPARSER_H
