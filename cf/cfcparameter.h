#ifndef CFCPARAMETER_H
#define CFCPARAMETER_H
#include <vector>

#include "treeutil.h"

/**
 * @brief parsed parameter from JSON
 */
class CFCParameter : public Node {
 public:
  CFCParameter(QString name, QString comment, QString feature,
               std::vector<QPointer<Node> > subnodes);

  std::vector<QPointer<Node> >& getSubnodes() override;

  QString getName() override;

  QString prettyPrint() override;

  QString getFeature();

 private:
  QString name;
  QString comment;
  QString feature;
  std::vector<QPointer<Node> > subnodes;
};

#endif  // CFCPARAMETER_H
