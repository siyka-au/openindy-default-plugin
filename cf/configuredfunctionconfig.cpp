#include "configuredfunctionconfig.h"

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
QList<QString> ConfiguredFunctionConfig::getFunctionNames() {
  switch (version) {
    case 1:
      return functionNames;
    case 2:
      QList<QString> list;

      collectFunctionNames(parameter->getSubnodes(), list);
      return list;
  }

  throw logic_error("illegal version");
}

void ConfiguredFunctionConfig::collectFunctionNames(
    QList<QPointer<Node> >& parameter, QList<QString>& names) {
  foreach (QPointer<Node> p, parameter) {
    if (!isNeededElement(p->getName())) {
      names.append(p->getName());
    }
    this->collectFunctionNames(p->getSubnodes(), names);
  }
}

QList<NeededElement> ConfiguredFunctionConfig::getNeededElements() {
  QList<NeededElement> list;
  for (auto& neededElement : this->neededElements) {
    list.append(neededElement.second);
  }
  return list;
}

QList<QString> ConfiguredFunctionConfig::getNeededElementNames() {
  QList<QString> list;
  for (auto& neededElement : this->neededElements) {
    list.append(neededElement.first);
  }
  return list;
}
