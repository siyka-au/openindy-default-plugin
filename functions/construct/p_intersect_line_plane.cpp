#include "p_intersect_line_plane.h"

#include <math.h>

#include <format>

/*!
 * \brief IntersectLinePlane::init
 */
void IntersectLinePlane::init() {
  // set plugin meta data
  this->metaData.name = "IntersectLinePlane";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{}",
      "This function calculates the intersection of a line with a plane.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select a line to calculate the intersection.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::eLineElement;
  NeededElement param2;
  param2.description = "Select a plane to calculate the intersection.";
  param2.infinite = false;
  param2.typeOfElement = ElementTypes::ePlaneElement;
  this->neededElements.push_back(param1);
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePointFeature);
}

/*!
 * \brief IntersectLinePlane::exec
 * \param point
 * \return
 */
std::expected<Point, std::string> IntersectLinePlane::exec() {
  Point point;
  if (!this->setUpResult(point)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return point;
}

/*!
 * \brief IntersectLinePlane::setUpResult
 * \param point
 * \return
 */
bool IntersectLinePlane::setUpResult(Point& point) {
  // get and check line and plane that shall be intersected
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1 ||
      !this->inputElements.contains(1) || this->inputElements[1].size() != 1) {
    return false;
  }
  oi::Guarded<Line> line = this->inputElements[0].at(0).line;
  oi::Guarded<Plane> plane = this->inputElements[1].at(0).plane;
  if (line.isNull() || plane.isNull() || !line->getIsSolved() ||
      !plane->getIsSolved()) {
    return false;
  }

  // check if line and plane intersect
  if (!this->isIntersection(line, plane)) {
    this->sendMessage(
        std::format("No intersection between line {} and plane {}",
                    line->getFeatureName(), plane->getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

  // perform intersection
  OiVec l_v = line->getDirection().getVector();   // direction vector for line
  OiVec p_n = plane->getDirection().getVector();  // normal vector for plane
  OiVec l_x0 = line->getPosition().getVector();   // position vector for line
  OiVec p_x0 = plane->getPosition().getVector();  // position vector for plane
  p_n.normalize();  // normalize normal vector for plane
  double d = 0.0;   // distance plane to origin
  OiVec::dot(d, p_x0, p_n);
  double h1 = NAN, h2 = NAN;
  OiVec::dot(h1, l_x0, p_n);
  OiVec::dot(h2, l_v, p_n);
  double lamda = (d - h1) / (h2);
  OiVec s = l_x0 + lamda * l_v;

  // set result
  Position section;
  section.setVector(s);
  point.setPoint(section);

  return true;
}

/*!
 * \brief IntersectLinePlane::isIntersection
 * \param line
 * \param plane
 * \return
 */
bool IntersectLinePlane::isIntersection(const oi::Guarded<Line>& line,
                                        const oi::Guarded<Plane>& plane) {
  // scalar product of the lines's direction vector and the plane's normal
  // vector
  double dot = 0.0;
  OiVec::dot(dot, line->getDirection().getVector(),
             plane->getDirection().getVector());
  if (dot > 0.0001 || dot < -0.0001) {
    return true;
  }
  return false;
}
