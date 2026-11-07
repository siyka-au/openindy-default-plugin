#include "p_intersectplaneplane.h"

#include <math.h>

#include <format>

/*!
 * \brief IntersectPlanePlane::init
 */
void IntersectPlanePlane::init() {
  // set plugin meta data
  this->metaData.name = "IntersectPlanePlane";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{}",
      "This function calculates the line of intersection of a plane with "
      "another plane.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select a plane to calculate the line of intersection.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::ePlaneElement;
  NeededElement param2;
  param2.description =
      "Select another plane to calculate the line intersection.";
  param2.infinite = false;
  param2.typeOfElement = ElementTypes::ePlaneElement;
  this->neededElements.push_back(param1);
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eLineFeature);
}

/*!
 * \brief IntersectPlanePlane::exec
 * \param line
 * \return
 */
std::expected<Line, std::string> IntersectPlanePlane::exec() {
  Line line;
  if (!this->setUpResult(line)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return line;
}

/*!
 * \brief IntersectPlanePlane::setUpResult
 * \param line
 * \return
 */
bool IntersectPlanePlane::setUpResult(Line& line) {
  // get and check planes that shall be intersected
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1 ||
      !this->inputElements.contains(1) || this->inputElements[1].size() != 1) {
    return false;
  }

  // check if input elements are equal
  if (this->inputElements[0].front().plane->getId() ==
      this->inputElements[1].front().plane->getId()) {
    this->sendMessage(std::string("Input geometries are equal."),
                      MessageTypes::eWarningMessage);
    return false;
  }

  oi::Guarded<Plane> plane1 = this->inputElements[0].at(0).plane;
  oi::Guarded<Plane> plane2 = this->inputElements[1].at(0).plane;
  if (plane1.isNull() || plane2.isNull() || !plane1->getIsSolved() ||
      !plane2->getIsSolved()) {
    return false;
  }

  // check if the two planes intersect
  if (!this->isIntersection(plane1, plane2)) {
    this->sendMessage(
        std::format("No intersection between plane {} and plane {}",
                    plane1->getFeatureName(), plane2->getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

  // perform intersection
  OiVec p1_n = plane1->getDirection().getVector();  // normal vector for plane 1
  OiVec p2_n = plane2->getDirection().getVector();  // normal vector for plane 2
  OiVec p1_x0 =
      plane1->getPosition().getVector();  // position vector for plane 1
  OiVec p2_x0 =
      plane2->getPosition().getVector();  // position vector for plane 2
  p1_n.normalize();  // normalized normal vector for plane 1
  p2_n.normalize();  // normalized normal vector for plane 2
  OiVec d(3);  // distance vector with distances origin to plane 1, 2, 3 (plane
               // 3 is normal to plane 1 and 2)
  double h1 = NAN, h2 = NAN;
  OiVec::dot(h1, p1_x0, p1_n);
  OiVec::dot(h2, p2_x0, p2_n);
  d.setAt(0, h1);
  d.setAt(1, h2);
  d.setAt(2, 0.0);
  OiVec l_v;  // vector normal to p1_n and p2_n
  OiVec::cross(l_v, p1_n, p2_n);
  OiVec l_x0;  // point on line of intersection
  this->calcPointOnLine(l_x0, p1_n, p2_n, l_v, d);

  // set result
  Position position;
  position.setVector(l_x0);
  Direction direction;
  direction.setVector(l_v);
  line.setLine(position, direction);
}

/*!
 * \brief IntersectPlanePlane::isIntersection
 * \param plane1
 * \param plane2
 * \return
 */
bool IntersectPlanePlane::isIntersection(const oi::Guarded<Plane>& plane1,
                                         const oi::Guarded<Plane>& plane2) {
  // scalar product of the normal vectors
  double dot = 0.0;
  OiVec::dot(dot, plane1->getDirection().getVector(),
             plane2->getDirection().getVector());
  if (dot > -0.99990 && dot < 0.99990) {
    return true;
  }
  return false;
}

/*!
 * \brief IntersectPlanePlane::calcPointOnLine
 * Calculate a point on the line of intersection by solving a system of
 * equations. The system of equations is given by the normal vectors of three
 * planes and their distances d.
 * \param p_l
 * \param n1
 * \param n2
 * \param n3
 * \param d
 */
void IntersectPlanePlane::calcPointOnLine(OiVec& p_l, const OiVec& n1,
                                          const OiVec& n2, const OiVec& n3,
                                          const OiVec& d) {
  OiMat a(3, 3);
  for (int i = 0; i < 3; i++) {
    a.setAt(0, i, n1.getAt(i));
    a.setAt(1, i, n2.getAt(i));
    a.setAt(2, i, n3.getAt(i));
  }
  p_l = a.inv() * d;
}
