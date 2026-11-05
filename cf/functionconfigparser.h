#ifndef FUNCTIONCONFIGPARSER_H
#define FUNCTIONCONFIGPARSER_H

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
  QList<ConfiguredFunctionConfig> readConfigFromJson(QString filename);
  QList<ConfiguredFunctionConfig> readConfigFromJson();

 private:
  QList<QPointer<Node> > parameter(QJsonValue value);

  QMultiMap<int, InputElementMapping> inputElementsMapping(
      ConfiguredFunctionConfig config, QJsonValue value);

  QList<QString> functionNames(QJsonValue value);

  ElementTypes typeOfElement(QJsonValue value);

  QList<QPair<QString, NeededElement> > neededElements(QJsonValue value);

  QList<FeatureTypes> applicableFor(QJsonValue value);

  QList<ConfiguredFunctionConfig> readConfigFromJson(QJsonObject json);
};

#endif  // FUNCTIONCONFIGPARSER_H
