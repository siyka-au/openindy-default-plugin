#ifndef P_BESTFITPLANE_H
#define P_BESTFITPLANE_H

#include <QObject>
#include <QPointer>
#include <string>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

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

#endif  // P_BESTFITPLANE_H
