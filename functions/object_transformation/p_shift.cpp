#include "p_shift.h"

#include <format>

#include "containers.h"

namespace {
constexpr double kMillimetresPerMetre = 1000.0;
}  // namespace

/*!
 * \brief Shift::init
 */
void Shift::init() {
  // set plugin meta data
  this->metaData.name = "Shift";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{}", "This function shifts the target geometry along its one vector.");
  this->metaData.iid = ObjectTransformation_iidd;

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePlaneFeature);
  this->applicableFor.push_back(FeatureTypes::eCircleFeature);

  // set double parameter
  this->doubleParameters.insert_or_assign("offset [mm]", 0.0);
}

/*!
 * \brief Shift::exec
 * \param plane
 * \return
 */
std::expected<Plane, std::string> Shift::exec(const Plane& input) {
  Plane plane = input;
  if (!this->setUpResult(plane)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return plane;
}

/*!
 * \brief Shift::exec
 * \param input
 * \return
 */
std::expected<Circle, std::string> Shift::exec(const Circle& input) {
  Circle circle = input;
  if (!this->setUpResult(circle)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return circle;
}

/*!
 * \brief Shift::setUpResult
 * \param plane
 * \return
 */
bool Shift::setUpResult(Plane& plane) {
  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / kMillimetresPerMetre;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / kMillimetresPerMetre;
  }

  // get normalized vector
  OiVec vec = plane.getDirection().getVector();
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
 * \brief Shift::setUpResult
 * \param circle
 * \return
 */
bool Shift::setUpResult(Circle& circle) {
  // get and check offset
  double offset = 0.0;
  if (this->scalarInputParams.doubleParameter.contains("offset [mm]")) {
    offset = oi::value(this->scalarInputParams.doubleParameter, "offset [mm]");
    offset = offset / kMillimetresPerMetre;
  } else {
    offset = oi::value(this->doubleParameters, "offset [mm]");
    offset = offset / kMillimetresPerMetre;
  }

  // get normalized vector
  OiVec vec = circle.getDirection().getVector();
  vec.normalize();

  // shift circle position
  OiVec x_circle = circle.getPosition().getVector();
  x_circle = x_circle + offset * vec;

  // set result
  Position position;
  position.setVector(x_circle);
  circle.setCircle(position, circle.getDirection(), circle.getRadius());

  return true;
}
