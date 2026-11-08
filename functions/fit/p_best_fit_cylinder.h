#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CYLINDER_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CYLINDER_H_

#include <QObject>
#include <QPointer>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

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

  std::expected<Cylinder, std::string> exec() override;

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

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_CYLINDER_H_
