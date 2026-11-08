#include "p_translate_by_value.h"

#include <format>

#include "containers.h"

/*!
 * \brief TranslateByValue::init
 */
void TranslateByValue::init() {
  // set plugin meta data
  this->metaData.name = "TranslateByValue";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{}", "This function shifts the target geometry along the input vector.");
  this->metaData.iid = ObjectTransformation_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select a geometry that has a vector.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::eDirectionElement;
  this->neededElements.push_back(param1);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePointFeature);
  this->applicableFor.push_back(FeatureTypes::eLineFeature);
  this->applicableFor.push_back(FeatureTypes::ePlaneFeature);
  this->applicableFor.push_back(FeatureTypes::eSphereFeature);

  // set double parameter
  this->doubleParameters.insert_or_assign("offset [mm]", 0.0);
}

/*!
 * \brief TranslateByValue::exec
 * \param point
 * \return
 */
std::expected<Point, std::string> TranslateByValue::exec(const Point& input) {
  Point point = input;
  if (!this->setUpResult(point)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return point;
}

/*!
 * \brief TranslateByValue::exec
 * \param input
 * \return
 */
std::expected<Line, std::string> TranslateByValue::exec(const Line& input) {
  Line line = input;
  if (!this->setUpResult(line)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return line;
}

/*!
 * \brief TranslateByValue::exec
 * \param input
 * \return
 */
std::expected<Plane, std::string> TranslateByValue::exec(const Plane& input) {
  Plane plane = input;
  if (!this->setUpResult(plane)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return plane;
}

/*!
 * \brief TranslateByValue::exec
 * \param input
 * \return
 */
std::expected<Sphere, std::string> TranslateByValue::exec(const Sphere& input) {
  Sphere sphere = input;
  if (!this->setUpResult(sphere)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return sphere;
}

/*!
 * \brief TranslateByValue::setUpResult
 * \param point
 * \return
 */
bool TranslateByValue::setUpResult(Point& point) {
  // get and check vector element
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    return false;
  }
  oi::Guarded<Geometry> geometry = this->inputElements[0].at(0).geometry;
  if (geometry.isNull() || !geometry->getIsSolved() ||
      !geometry->hasDirection()) {
    return false;
  }

  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // get normalized vector
  OiVec vec = geometry->getDirection().getVector();
  vec.normalize();

  // shift point position
  OiVec x_point = point.getPosition().getVector();
  x_point = x_point + offset * vec;

  // set result
  Position position;
  position.setVector(x_point);
  point.setPoint(position);

  return true;
}

/*!
 * \brief TranslateByValue::setUpResult
 * \param line
 * \return
 */
bool TranslateByValue::setUpResult(Line& line) {
  // get and check vector element
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    return false;
  }
  oi::Guarded<Geometry> geometry = this->inputElements[0].at(0).geometry;
  if (geometry.isNull() || !geometry->getIsSolved() ||
      !geometry->hasDirection()) {
    return false;
  }

  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // get normalized vector
  OiVec vec = geometry->getDirection().getVector();
  vec.normalize();

  // shift line position
  OiVec x_line = line.getPosition().getVector();
  x_line = x_line + offset * vec;

  // set result
  Position position;
  position.setVector(x_line);
  line.setLine(position, line.getDirection());

  return true;
}

/*!
 * \brief TranslateByValue::setUpResult
 * \param plane
 * \return
 */
bool TranslateByValue::setUpResult(Plane& plane) {
  // get and check vector element
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    return false;
  }
  oi::Guarded<Geometry> geometry = this->inputElements[0].at(0).geometry;
  if (geometry.isNull() || !geometry->getIsSolved() ||
      !geometry->hasDirection()) {
    return false;
  }

  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // get normalized vector
  OiVec vec = geometry->getDirection().getVector();
  vec.normalize();

  // shift plane position
  OiVec x_plane = plane.getPosition().getVector();
  x_plane = x_plane + offset * vec;

  // set result
  Position position;
  position.setVector(x_plane);
  plane.setPlane(position, plane.getDirection());

  return true;
}

/*!
 * \brief TranslateByValue::setUpResult
 * \param sphere
 * \return
 */
bool TranslateByValue::setUpResult(Sphere& sphere) {
  // get and check vector element
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    return false;
  }
  oi::Guarded<Geometry> geometry = this->inputElements[0].at(0).geometry;
  if (geometry.isNull() || !geometry->getIsSolved() ||
      !geometry->hasDirection()) {
    return false;
  }

  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // get normalized vector
  OiVec vec = geometry->getDirection().getVector();
  vec.normalize();

  // shift sphere position
  OiVec x_sphere = sphere.getPosition().getVector();
  x_sphere = x_sphere + offset * vec;

  // set result
  Position position;
  position.setVector(x_sphere);
  sphere.setSphere(position, sphere.getRadius());

  return true;
}
