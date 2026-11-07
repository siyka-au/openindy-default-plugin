#ifndef CONFIGUREDFUNCTIONCONFIG_H
#define CONFIGUREDFUNCTIONCONFIG_H
#include <map>
#include <vector>

#include "constructfunction.h"
#include "featurewrapper.h"
#include "function.h"
#include "treeutil.h"
#include "types.h"

using namespace oi;

struct InputElementMapping {
  int functionIndex;
  int srcInputElementIndex;
  int dstInputElementIndex;
};

class ConfiguredFunctionConfig {
 public:
  ConfiguredFunctionConfig();

 public:
  bool isNeededElement(QString name);
  bool isFunction(QString name);
  /**
   * @brief getFunctionNames
   * @return all necessary funktions for this config
   */
  std::vector<QString> getFunctionNames();
  void collectFunctionNames(std::vector<QPointer<Node> >& parameter,
                            std::vector<QString>& names);
  std::vector<NeededElement> getNeededElements();
  std::vector<QString> getNeededElementNames();

 public:
  int version{};
  QString name;
  QString description;
  std::vector<QPair<QString, NeededElement> > neededElements;
  std::vector<FeatureTypes> applicableFor;

  // versionn 1
  std::vector<QString> functionNames;
  std::multimap<int, InputElementMapping> inputElementsMapping;

  // version 2
  QPointer<Node> parameter;
};

#endif  // CONFIGUREDFUNCTIONCONFIG_H
