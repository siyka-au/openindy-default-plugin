#include "configuredfunctionconfig.h"

#include <QtCore>
#include <utility>
#include <vector>

ConfiguredFunctionConfig::ConfiguredFunctionConfig() {}

bool ConfiguredFunctionConfig::isNeededElement(std::string name) {
  for (int i = 0; std::cmp_less(i, this->neededElements.size()); i++) {
    if (neededElements[i].first.compare(name) == 0) {
      return true;
    }
  }
  return false;
}

bool ConfiguredFunctionConfig::isFunction(std::string name) {
  return !isNeededElement(name);
}

/**
 * @brief getFunctionNames
 * @return all necessary funktions for this config
 */
std::vector<std::string> ConfiguredFunctionConfig::getFunctionNames() {
  switch (version) {
    case 1:
      return functionNames;
    case 2:
      std::vector<std::string> list;

      collectFunctionNames(parameter->getSubnodes(), list);
      return list;
  }

  throw logic_error("illegal version");
}

void ConfiguredFunctionConfig::collectFunctionNames(
    std::vector<QPointer<Node> >& parameter, std::vector<std::string>& names) {
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

std::vector<std::string> ConfiguredFunctionConfig::getNeededElementNames() {
  std::vector<std::string> list;
  for (auto& neededElement : this->neededElements) {
    list.push_back(neededElement.first);
  }
  return list;
}
