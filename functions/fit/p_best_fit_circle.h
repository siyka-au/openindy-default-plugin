#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_H_

#include <span>
#include <vector>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The BestFitCircle class
 */
class BestFitCircle : public FitFunction, public Producer<Circle> {
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
  bool setUpResult(Circle& circle);

  OiVec getApproximatedPlane(int nop, std::span<const double> x,
                             std::span<const double> y,
                             std::span<const double> z, double cx, double cy,
                             double cz);
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CIRCLE_H_
