#ifndef P_POINTFROMPOINTS_H
#define P_POINTFROMPOINTS_H

#include <QObject>
#include <QPointer>
#include <string>

#include "construct_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;
using namespace std;

/*!
 * \brief The PointFromPoints class
 */
class PointFromPoints : public ConstructFunction, public Producer<Point> {
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

#endif  // P_POINTFROMPOINTS_H
