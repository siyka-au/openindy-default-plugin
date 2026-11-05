#ifndef P_BESTFITCIRCLE_H
#define P_BESTFITCIRCLE_H

#include <span>
#include <vector>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

/*!
 * \brief The BestFitCircle class
 */
class BestFitCircle : public FitFunction, public Producer<Circle> {
  Q_OBJECT

 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Circle, QString> exec() override;

 private:
  bool setUpResult(Circle& circle);

  OiVec getApproximatedPlane(int nop, std::span<const double> x,
                             std::span<const double> y,
                             std::span<const double> z, double cx, double cy,
                             double cz);
};

#endif  // P_BESTFITCIRCLE_H
