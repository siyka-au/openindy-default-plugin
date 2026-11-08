#ifndef OI_DEFAULT_PLUGIN_CF_CFC_PARAMETER_H_
#define OI_DEFAULT_PLUGIN_CF_CFC_PARAMETER_H_
#include <QtCore>
#include <vector>

#include "tree_util.h"

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

#endif  // OI_DEFAULT_PLUGIN_CF_CFC_PARAMETER_H_
