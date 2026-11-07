#ifndef FUNCTIONCONFIGPARSER_H
#define FUNCTIONCONFIGPARSER_H
#include <map>
#include <vector>

#include "cfcontext.h"
#include "cfcparameter.h"
#include "configuredfunctionconfig.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "treeutil.h"
#include "types.h"

using namespace oi;

/**
 * @brief parses JSON config file and generate list of FunctionConfigParser
 */
class FunctionConfigParser {
 public:
  std::vector<ConfiguredFunctionConfig> readConfigFromJson(QString filename);
  std::vector<ConfiguredFunctionConfig> readConfigFromJson();

 private:
  std::vector<QPointer<Node> > parameter(QJsonValue value);

  std::multimap<int, InputElementMapping> inputElementsMapping(
      ConfiguredFunctionConfig config, QJsonValue value);

  std::vector<QString> functionNames(QJsonValue value);

  ElementTypes typeOfElement(QJsonValue value);

  std::vector<QPair<QString, NeededElement> > neededElements(QJsonValue value);

  std::vector<FeatureTypes> applicableFor(QJsonValue value);

  std::vector<ConfiguredFunctionConfig> readConfigFromJson(QJsonObject json);
};

#endif  // FUNCTIONCONFIGPARSER_H
