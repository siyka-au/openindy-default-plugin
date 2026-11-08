#include "p_change_radius.h"

#include <format>

#include "containers.h"

/*!
 * \brief ChangeRadius::init
 */
void ChangeRadius::init() {
  // set plugin meta data
  this->metaData.name = "ChangeRadius";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{}",
      "This functions adds the specified offset to the radius of the target "
      "geometry");
  this->metaData.iid = ObjectTransformation_iidd;

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eSphereFeature);
  this->applicableFor.push_back(FeatureTypes::eCircleFeature);
  this->applicableFor.push_back(FeatureTypes::eCylinderFeature);

  // set double parameter
  this->doubleParameters.insert_or_assign("offset [mm]", 0.0);
}

/*!
 * \brief ChangeRadius::exec
 * \param sphere
 * \return
 */
std::expected<Sphere, std::string> ChangeRadius::exec(const Sphere& input) {
  Sphere sphere = input;
  if (!this->setUpResult(sphere)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return sphere;
}

/*!
 * \brief ChangeRadius::exec
 * \param input
 * \return
 */
std::expected<Circle, std::string> ChangeRadius::exec(const Circle& input) {
  Circle circle = input;
  if (!this->setUpResult(circle)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return circle;
}

/*!
 * \brief ChangeRadius::exec
 * \param input
 * \return
 */
std::expected<Cylinder, std::string> ChangeRadius::exec(const Cylinder& input) {
  Cylinder cylinder = input;
  if (!this->setUpResult(cylinder)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return cylinder;
}

/*!
 * \brief ChangeRadius::setUpResult
 * \param sphere
 * \return
 */
bool ChangeRadius::setUpResult(Sphere& sphere) {
  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // add offset to radius of the sphere
  Position position = sphere.getPosition();
  Radius radius = sphere.getRadius();
  radius.setRadius(radius.getRadius() + offset);

  // set result
  sphere.setSphere(position, radius);

  return true;
}

/*!
 * \brief ChangeRadius::setUpResult
 * \param circle
 * \return
 */
bool ChangeRadius::setUpResult(Circle& circle) {
  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // add offset to radius of the circle
  Position position = circle.getPosition();
  Direction direction = circle.getDirection();
  Radius radius = circle.getRadius();
  radius.setRadius(radius.getRadius() + offset);

  // set result
  circle.setCircle(position, direction, radius);

  return true;
}

/*!
 * \brief ChangeRadius::setUpResult
 * \param cylinder
 * \return
 */
bool ChangeRadius::setUpResult(Cylinder& cylinder) {
  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / 1000.0;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / 1000.0;
  }

  // add offset to radius of the cylinder
  Position position = cylinder.getPosition();
  Direction direction = cylinder.getDirection();
  Radius radius = cylinder.getRadius();
  radius.setRadius(radius.getRadius() + offset);

  // set result
  cylinder.setCylinder(position, direction, radius);

  return true;
}
