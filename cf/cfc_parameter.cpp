#include "cfc_parameter.h"

#include <QtCore>
#include <vector>

CFCParameter::CFCParameter(std::string name, std::string comment,
                           std::string feature,
                           std::vector<QPointer<Node> > subnodes)
    : name(name), comment(comment), feature(feature), subnodes(subnodes) {}

std::vector<QPointer<Node> >& CFCParameter::getSubnodes() { return subnodes; }

std::string CFCParameter::getName() { return name; }

std::string CFCParameter::getFeature() { return feature; }

std::string CFCParameter::prettyPrint() {
  std::string s;
  s += "{ name: '" + this->name + "', feature: '" + this->feature +
       "', comment: '" + this->comment + "'";
  if (!this->subnodes.empty()) {
    s += ", parameter: [ ";
    for (QPointer<Node> p : this->subnodes) {
      s += " " + p->prettyPrint();
    }
    s += " ], ";
  }
  s += "}, ";
  return s;
}
