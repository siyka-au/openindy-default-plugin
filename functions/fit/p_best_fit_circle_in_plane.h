#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_IN_PLANE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_IN_PLANE_H_

#include <QObject>
#include <QPointer>
#include <string>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;
using namespace std;

/*!
 * \brief The BestFitCircleInPlane class: plane by principal components, then
 * the geometric (orthogonal distance) least-squares circle in that plane.
 */
class BestFitCircleInPlane : public FitFunction,
                             public BestFitCircleUtil,
                             public Producer<Circle> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  BestFitCircleInPlane() = default;

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Circle, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Circle& circle);

 protected:
  explicit BestFitCircleInPlane(bool geometric) : geometricFit(geometric) {}

 private:
  //! true: orthogonal-distance (geometric) circle; false: algebraic (Kasa)
  bool geometricFit = true;
};

/*!
 * \brief The BestFitCircleInPlaneAlgebraic class: the same plane-then-circle
 * fit with the algebraic (Kasa) circle instead of the geometric one.
 */
class BestFitCircleInPlaneAlgebraic : public BestFitCircleInPlane {
 public:
  BestFitCircleInPlaneAlgebraic() : BestFitCircleInPlane(false) {}
  void init() override;
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_IN_PLANE_H_
