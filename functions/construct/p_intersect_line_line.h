#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_CONSTRUCT_P_INTERSECT_LINE_LINE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_CONSTRUCT_P_INTERSECT_LINE_LINE_H_

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

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_CONSTRUCT_P_INTERSECT_LINE_LINE_H_
