#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_CONSTRUCT_P_INTERSECT_PLANE_PLANE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_CONSTRUCT_P_INTERSECT_PLANE_PLANE_H_

#include <QObject>
#include <QPointer>

#include "construct_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The IntersectPlanePlane class
 */
class IntersectPlanePlane : public ConstructFunction, public Producer<Line> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Line, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Line& line);
  bool isIntersection(const oi::Guarded<Plane>& plane1,
                      const oi::Guarded<Plane>& plane2);
  void calcPointOnLine(OiVec& p_l, const OiVec& n1, const OiVec& n2,
                       const OiVec& n3, const OiVec& d);
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_CONSTRUCT_P_INTERSECT_PLANE_PLANE_H_
