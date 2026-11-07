#ifndef P_BESTFTICIRCLEINPLANE_H
#define P_BESTFTICIRCLEINPLANE_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

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

  std::expected<Circle, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Circle& circle);
};

#endif  // P_BESTFTICIRCLEINPLANE_H
