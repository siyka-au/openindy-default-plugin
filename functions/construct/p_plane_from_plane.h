#ifndef P_PLANEFROMPLANE_H
#define P_PLANEFROMPLANE_H

#include <QObject>
#include <QPointer>

#include "construct_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The PlaneFromPlane class
 */
class PlaneFromPlane : public ConstructFunction, public Producer<Plane> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Plane, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Plane& plane);
};

#endif  // P_PLANEFROMPLANE_H
