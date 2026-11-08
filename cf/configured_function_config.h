#ifndef OI_DEFAULT_PLUGIN_CF_CONFIGURED_FUNCTION_CONFIG_H_
#define OI_DEFAULT_PLUGIN_CF_CONFIGURED_FUNCTION_CONFIG_H_
#include <QtCore>
#include <map>
#include <vector>

#include "construct_function.h"
#include "feature_wrapper.h"
#include "function.h"
#include "tree_util.h"
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
  bool isNeededElement(std::string name);
  bool isFunction(std::string name);
  /**
   * @brief getFunctionNames
   * @return all necessary funktions for this config
   */
  std::vector<std::string> getFunctionNames();
  void collectFunctionNames(std::vector<QPointer<Node> >& parameter,
                            std::vector<std::string>& names);
  std::vector<NeededElement> getNeededElements();
  std::vector<std::string> getNeededElementNames();

 public:
  int version{};
  std::string name;
  std::string description;
  std::vector<QPair<std::string, NeededElement> > neededElements;
  std::vector<FeatureTypes> applicableFor;

  // versionn 1
  std::vector<std::string> functionNames;
  std::multimap<int, InputElementMapping> inputElementsMapping;

  // version 2
  QPointer<Node> parameter;
};

#endif  // OI_DEFAULT_PLUGIN_CF_CONFIGURED_FUNCTION_CONFIG_H_
