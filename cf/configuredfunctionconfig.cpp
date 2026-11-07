#include "configuredfunctionconfig.h"

#include <vector>

ConfiguredFunctionConfig::ConfiguredFunctionConfig() {}

bool ConfiguredFunctionConfig::isNeededElement(QString name) {
  for (int i = 0; i < this->neededElements.size(); i++) {
    if (neededElements[i].first.compare(name) == 0) {
      return true;
    }
  }
  return false;
}

bool ConfiguredFunctionConfig::isFunction(QString name) {
  return !isNeededElement(name);
}

/**
 * @brief getFunctionNames
 * @return all necessary funktions for this config
 */
std::vector<QString> ConfiguredFunctionConfig::getFunctionNames() {
  switch (version) {
    case 1:
      return functionNames;
    case 2:
      std::vector<QString> list;

      collectFunctionNames(parameter->getSubnodes(), list);
      return list;
  }

  throw logic_error("illegal version");
}

void ConfiguredFunctionConfig::collectFunctionNames(
    std::vector<QPointer<Node> >& parameter, std::vector<QString>& names) {
  for (QPointer<Node> p : parameter) {
    if (!isNeededElement(p->getName())) {
      names.push_back(p->getName());
    }
    this->collectFunctionNames(p->getSubnodes(), names);
  }
}

std::vector<NeededElement> ConfiguredFunctionConfig::getNeededElements() {
  std::vector<NeededElement> list;
  for (auto& neededElement : this->neededElements) {
    list.push_back(neededElement.second);
  }
  return list;
}

std::vector<QString> ConfiguredFunctionConfig::getNeededElementNames() {
  std::vector<QString> list;
  for (auto& neededElement : this->neededElements) {
    list.push_back(neededElement.first);
  }
  return list;
}
