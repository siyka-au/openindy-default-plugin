#ifndef CFCPARAMETER_H
#define CFCPARAMETER_H
#include <QtCore>
#include <vector>

#include "treeutil.h"

/**
 * @brief parsed parameter from JSON
 */
class CFCParameter : public Node {
 public:
  CFCParameter(std::string name, std::string comment, std::string feature,
               std::vector<QPointer<Node> > subnodes);

  std::vector<QPointer<Node> >& getSubnodes() override;

  std::string getName() override;

  std::string prettyPrint() override;

  std::string getFeature();

 private:
  std::string name;
  std::string comment;
  std::string feature;
  std::vector<QPointer<Node> > subnodes;
};

#endif  // CFCPARAMETER_H
