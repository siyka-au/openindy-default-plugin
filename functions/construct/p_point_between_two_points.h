#ifndef P_POINTBETWEENTWOPOINTS_H
#define P_POINTBETWEENTWOPOINTS_H

#include <QObject>
#include <QPointer>

#include "construct_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The PointBetweenTwoPoints class
 */
class PointBetweenTwoPoints : public ConstructFunction, public Producer<Point> {
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
};

#endif  // P_POINTBETWEENTWOPOINTS_H
