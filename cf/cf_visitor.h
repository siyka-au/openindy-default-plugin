#ifndef OI_DEFAULT_PLUGIN_CF_CF_VISITOR_H_
#define OI_DEFAULT_PLUGIN_CF_CF_VISITOR_H_

#include <QtCore>

#include "cf_context.h"
#include "cf_function_data.h"
#include "tree_util.h"

using namespace oi;

/**
 * @brief Visitor for CFCParameter
 */
class CFVisitor : public NodeVisitor {
 public:
  explicit CFVisitor(CFContext ctx);

  void pre(QPointer<Node> node, int index, int level) override;
  void post(QPointer<Node> node, int index, int level) override;

 private:
  CFContext ctx;
  // data for each function stack is pushed / poped while traversal the node
  // tree
  QStack<CFFunctionData> data;
};

#endif  // OI_DEFAULT_PLUGIN_CF_CF_VISITOR_H_
