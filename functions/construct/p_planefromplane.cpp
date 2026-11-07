#include "p_planefromplane.h"

#include <format>

/*!
 * \brief PlaneFromPlane::init
 */
void PlaneFromPlane::init() {
  // set plugin meta data
  this->metaData.name = "PlaneFromPlane";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description =
      std::format("{}", "This function creates a copy of another plane.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select the plane to copy.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::ePlaneElement;
  this->neededElements.push_back(param1);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePlaneFeature);
}

/*!
 * \brief PlaneFromPlane::exec
 * \param plane
 * \return
 */
std::expected<Plane, std::string> PlaneFromPlane::exec() {
  Plane plane;
  if (!this->setUpResult(plane)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return plane;
}

/*!
 * \brief PlaneFromPlane::setUpResult
 * \param plane
 * \return
 */
bool PlaneFromPlane::setUpResult(Plane& plane) {
  // get and check input plane
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    return false;
  }

  oi::Guarded<Plane> inputPlane = this->inputElements[0].at(0).plane;
  if (inputPlane.isNull() || !inputPlane->getIsSolved()) {
    this->sendMessage(std::format("Not enough valid input to copy the plane {}",
                                  plane.getFeatureName()),
                      MessageTypes::eWarningMessage);
    return false;
  }

  this->setIsUsed(0, this->inputElements[0].at(0).id, true);

  // set result
  plane.setPlane(inputPlane->getPosition(), inputPlane->getDirection());

  // set statistic
  plane.setStatistic(inputPlane->getStatistic());

  return true;
}
