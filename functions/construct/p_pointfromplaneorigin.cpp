#include "p_pointfromplaneorigin.h"

#include <format>

/*!
 * \brief PointFromPlaneOrigin::init
 */
void PointFromPlaneOrigin::init() {
  // set plugin meta data
  this->metaData.name = "PointFromPlaneOrigin";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "oi";
  this->metaData.description = std::format(
      "{} {}",
      "This function creates a point at the position of a plane, i.e. "
      "the centroid its fit solved to.",
      "The point follows the plane: change or re-measure the plane and "
      "the point updates with it.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select the plane whose position defines the point.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::ePlaneElement;
  this->neededElements.push_back(param1);

  // set applicable for
  this->applicableFor.push_back(FeatureTypes::ePointFeature);
}

/*!
 * \brief PointFromPlaneOrigin::exec
 * \param point
 * \return
 */
std::expected<Point, std::string> PointFromPlaneOrigin::exec() {
  Point point;
  if (!this->setUpResult(point)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return point;
}

/*!
 * \brief PointFromPlaneOrigin::setUpResult
 * \param point
 * \return
 */
bool PointFromPlaneOrigin::setUpResult(Point& point) {
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    this->sendMessage(
        std::format("No plane set for point {}", point.getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

  const InputElement& element = this->inputElements[0].front();

  if (element.plane.isNull() || !element.plane->getIsSolved()) {
    this->setIsUsed(0, element.id, false);
    this->sendMessage(std::format("The plane for point {} is not solved",
                                  point.getFeatureName()),
                      MessageTypes::eWarningMessage);
    return false;
  }

  this->setIsUsed(0, element.id, true);

  Position pointPosition = element.plane->getPosition();
  point.setPoint(pointPosition);

  return true;
}
