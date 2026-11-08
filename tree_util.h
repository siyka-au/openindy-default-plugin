#ifndef OI_DEFAULT_PLUGIN_TREE_UTIL_H_
#define OI_DEFAULT_PLUGIN_TREE_UTIL_H_
#include <QDebug>
#include <QObject>
#include <QPointer>
#include <format>
#include <string>
#include <utility>
#include <vector>

class Node : public QObject {
  Q_OBJECT

 public:
  virtual std::string getName() = 0;
  virtual std::string prettyPrint() = 0;
  virtual std::vector<QPointer<Node> >& getSubnodes() = 0;
};

class NodeVisitor : public QObject {
  Q_OBJECT

 public:
  virtual void pre(QPointer<Node> node, int index, int level) = 0;
  virtual void post(QPointer<Node> node, int index, int level) = 0;

  void debug(std::string label, std::string message, int level) {
    qDebug() << QString::fromStdString(std::format(
        "{}{}: {}", std::string(static_cast<std::size_t>(level) * 2, ' '),
        label, message));
  }
};

/**
 * common visitor to pretty print the current node
 *
 * @brief The PrintVisitor class
 */
class PrintVisitor : public NodeVisitor {
 public:
  void pre(QPointer<Node> node, int index, int level) override {
    debug(std::format("pre L{} I{}", level, index), node->prettyPrint(), level);
  }

  void post(QPointer<Node> node, int index, int level) override {
    debug(std::format("post L{} I{}", level, index), node->prettyPrint(),
          level);
  }
};

class ListVisitor : public NodeVisitor {
 public:
  void pre(QPointer<Node> node, int index, int level) override {
    for (QPointer<NodeVisitor> v : list) {
      v->pre(node, index, level);
    }
  }

  void post(QPointer<Node> node, int index, int level) override {
    for (QPointer<NodeVisitor> v : list) {
      v->post(node, index, level);
    }
  }

  std::vector<QPointer<NodeVisitor> > list;
};

class TreeUtil {
 public:
  static void traversal(QPointer<Node> node, NodeVisitor& h, int index = 0,
                        int level = 0) {
    h.pre(node, index, level);

    for (int idx = 0; std::cmp_less(idx, node->getSubnodes().size()); idx++) {
      int lvl = level;
      traversal(node->getSubnodes()[idx], h, idx, ++lvl);
    }

    h.post(node, index, level);
  }
};

#endif  // OI_DEFAULT_PLUGIN_TREE_UTIL_H_
