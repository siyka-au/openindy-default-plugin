#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_PLANE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_PLANE_H_

#include <QObject>
#include <QPointer>
#include <string>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The BestFitPlane class
 */
class BestFitPlane : public FitFunction,
                     public BestFitPlaneUtil,
                     public Producer<Plane> {
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

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_PLANE_H_
