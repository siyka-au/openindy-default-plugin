#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_SPHERE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_SPHERE_H_
#include <QObject>
#include <QPointer>
#include <string>
#include <vector>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The BestFitSphere class
 */
class BestFitSphere : public FitFunction, public Producer<Sphere> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Sphere, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Sphere& sphere);
  bool approximate(Sphere& sphere,
                   const std::vector<oi::Guarded<Point> >& inputPoints);
  bool fit(Sphere& sphere, const std::vector<oi::Guarded<Point> >& inputPoints,
           const std::vector<oi::Guarded<Point> >& allUsablePoints);
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_SPHERE_H_
