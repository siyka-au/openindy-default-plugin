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
 * \brief The BestFitCircleInPlane class
 */
class BestFitCircleInPlane : public FitFunction,
                             public BestFitCircleUtil,
                             public Producer<Circle> {
 public:
  // ##############################
  // function initialization method
  // ##############################

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
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_IN_PLANE_H_
