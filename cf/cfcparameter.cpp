#include "cfcparameter.h"

#include <vector>

CFCParameter::CFCParameter(QString name, QString comment, QString feature,
                           std::vector<QPointer<Node> > subnodes)
    : name(name), comment(comment), feature(feature), subnodes(subnodes) {}

std::vector<QPointer<Node> >& CFCParameter::getSubnodes() { return subnodes; }

QString CFCParameter::getName() { return name; }

QString CFCParameter::getFeature() { return feature; }

QString CFCParameter::prettyPrint() {
  QString s;
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
