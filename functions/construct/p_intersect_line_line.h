#ifndef P_INTERSECTLINELINE_H
#define P_INTERSECTLINELINE_H

#include <QObject>
#include <QPointer>

#include "construct_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

enum class IntersectionTypes { eAtFirstLine, eAtSecondLine, eMidPoint };

/*!
 * \brief The IntersectLineline class
 */
class IntersectLineLine : public ConstructFunction, public Producer<Point> {
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
  bool isParallel(const oi::Guarded<Line>& line1,
                  const oi::Guarded<Line>& line2);
  IntersectionTypes getIntersectionType();
};

#endif  // P_INTERSECTLINELINE_H
