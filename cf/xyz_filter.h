#ifndef XYZFILTER_H
#define XYZFILTER_H

#include <QObject>

#include "construct_function.h"
#include "feature_wrapper.h"
#include "oi_vec.h"

using namespace oi;

/**
 * @brief set axis to 0 and keeps others
 */
class XYZFilter : public ConstructFunction, public Transform<Point> {
 public:
  XYZFilter(std::string name, bool removeX, bool removeY, bool removeZ);

  void init() override;

  std::expected<Point, std::string> exec(const Point& input) override;

 private:
  bool removeX;
  bool removeY;
  bool removeZ;
};

#endif  // XYZFILTER_H
