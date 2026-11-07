#ifndef P_BESTFITCYLINDER_H
#define P_BESTFITCYLINDER_H

#include <QObject>
#include <QPointer>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

/*!
 * \brief The BestFitCylinder class
 */
class BestFitCylinder : public FitFunction,
                        public BestFitCylinderUtil,
                        public Producer<Cylinder> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Cylinder, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Cylinder& cylinder);
};

class BestFitCylinderAppxDirection : public BestFitCylinder {
 public:
  void init() override;
};

class BestFitCylinderAppxDummyPoint : public BestFitCylinder {
 public:
  void init() override;
};

#endif  // P_BESTFITCYLINDER_H
