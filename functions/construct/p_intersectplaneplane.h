#ifndef P_INTERSECTPLANEPLANE_H
#define P_INTERSECTPLANEPLANE_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"
#include "oimat.h"
#include "oivec.h"

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

#endif  // P_INTERSECTPLANEPLANE_H
