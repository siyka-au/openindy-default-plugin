#ifndef P_INTERSECTLINEPLANE_H
#define P_INTERSECTLINEPLANE_H

#include <QObject>
#include <QPointer>

#include "construct_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The IntersectLinePlane class
 */
class IntersectLinePlane : public ConstructFunction, public Producer<Point> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Point, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Point& point);
  bool isIntersection(const oi::Guarded<Line>& line,
                      const oi::Guarded<Plane>& plane);
};

#endif  // P_INTERSECTLINEPLANE_H
