#include "p_rectifytopoint.h"

#include <math.h>

#include <format>

#include "containers.h"

/*!
 * \brief RectifyToPoint::init
 */
void RectifyToPoint::init() {
  // set plugin meta data
  this->metaData.name = "RectifyToPoint";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{}",
      "This function changes the orientation of a geometry according to the "
      "specified position.");
  this->metaData.iid = ObjectTransformation_iidd;

  // set needed elements
  NeededElement param1;
  param1.description =
      "Select a position used for the orientation of the target geometry.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::ePositionElement;
  this->neededElements.push_back(param1);

  NeededElement param2;
  param2.description =
      "Select a station used for the orientation of the target geometry.";
  param2.infinite = false;
  param2.typeOfElement = ElementTypes::eStationElement;
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePlaneFeature);
  this->applicableFor.push_back(FeatureTypes::eCircleFeature);
  this->applicableFor.push_back(FeatureTypes::eLineFeature);
  this->applicableFor.push_back(FeatureTypes::eCylinderFeature);

  // set string parameter
  this->stringParameters.emplace("sense", "negative");
  this->stringParameters.emplace("sense", "positive");
}

/*!
 * \brief RectifyToPoint::exec
 * \param plane
 * \return
 */
std::expected<Plane, std::string> RectifyToPoint::exec(const Plane& input) {
  Plane plane = input;
  if (!this->setUpResult(plane)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return plane;
}

/*!
 * \brief RectifyToPoint::exec
 * \param input
 * \return
 */
std::expected<Circle, std::string> RectifyToPoint::exec(const Circle& input) {
  Circle circle = input;
  if (!this->setUpResult(circle)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return circle;
}

/*!
 * \brief RectifyToPoint::exec
 * \param input
 * \return
 */
std::expected<Line, std::string> RectifyToPoint::exec(const Line& input) {
  Line line = input;
  if (!this->setUpResult(line)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return line;
}

/*!
 * \brief RectifyToPoint::exec
 * \param input
 * \return
 */
std::expected<Cylinder, std::string> RectifyToPoint::exec(
    const Cylinder& input) {
  Cylinder cylinder = input;
  if (!this->setUpResult(cylinder)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return cylinder;
}

/*!
 * \brief RectifyToPoint::point
 * \param point
 * \return if valid point was found
 */
bool RectifyToPoint::point(OiVec& point) {
  // 1. get and check position of rectify geometry if available
  if (this->inputElements.contains(0) && this->inputElements[0].size() == 1) {
    oi::Guarded<Geometry> rectifyGeometry =
        this->inputElements[0].at(0).geometry;
    if (!rectifyGeometry.isNull() && rectifyGeometry->getIsSolved() &&
        rectifyGeometry->hasPosition()) {
      point = rectifyGeometry->getPosition().getVector();
      return true;
    }
  } else {  // 2. get and check position of rectify station if available
    if (this->inputElements.contains(1) && this->inputElements[1].size() == 1) {
      oi::Guarded<Station> rectifyStation =
          this->inputElements[1].at(0).station;
      if (!rectifyStation.isNull() && !rectifyStation->getPosition().isNull() &&
          rectifyStation->getPosition()->hasPosition()) {
        point = rectifyStation->getPosition()->getPosition().getVector();
        return true;
      }
    }
  }

  return false;
}

/*!
 * \brief RectifyToPoint::setUpResult
 * \param plane
 * \return
 */
bool RectifyToPoint::setUpResult(Geometry& geometry) {
  OiVec p;
  if (!point(p)) {
    return false;
  }

  // get the sense (positive or negative)
  double sense = 1.0;
  if (this->scalarInputParams.stringParameter.contains("sense")) {
    if (oi::value(this->scalarInputParams.stringParameter, "sense")
            .compare("negative") == 0) {
      sense = -1.0;
    }
  }

  // get the position of the point and the plane and the normal vector
  OiVec n = geometry.getDirection().getVector();
  n.normalize();
  OiVec x = geometry.getPosition().getVector();

  // calculate the distance of the plane from the origin
  double d = NAN;
  OiVec::dot(d, x, n);
  if (d < 0.0) {
    n = -1.0 * n;
    d = -d;
  }

  // calculate the distance of the point position from the plane
  double s = NAN;
  OiVec::dot(s, p, n);
  s = s - d;

  // invert the distance if sense is negative
  s = s * sense;

  // invert the normal vector of the plane if it has the wrong orientation
  if (s < 0.0) {
    n = -1.0 * n;
  }

  // set result
  Direction direction = geometry.getDirection();
  direction.setVector(n);
  geometry.setDirection(direction);

  return true;
}
