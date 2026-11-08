#include "p_line_from_plane_normal.h"

#include <format>

/*!
 * \brief LineFromPlaneNormal::init
 */
void LineFromPlaneNormal::init() {
  // set plugin meta data
  this->metaData.name = "LineFromPlaneNormal";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "oi";
  this->metaData.description = std::format(
      "{} {}",
      "This function creates a line along the normal of a plane, "
      "through the plane's position.",
      "The line follows the plane: change or re-measure the plane and "
      "the line updates with it.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select the plane whose normal defines the line.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::ePlaneElement;
  this->neededElements.push_back(param1);

  // set applicable for
  this->applicableFor.push_back(FeatureTypes::eLineFeature);
}

/*!
 * \brief LineFromPlaneNormal::exec
 * \param line
 * \return
 */
std::expected<Line, std::string> LineFromPlaneNormal::exec() {
  Line line;
  if (!this->setUpResult(line)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return line;
}

/*!
 * \brief LineFromPlaneNormal::setUpResult
 * \param line
 * \return
 */
bool LineFromPlaneNormal::setUpResult(Line& line) {
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1) {
    this->sendMessage(
        std::format("No plane set for line {}", line.getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

  const InputElement& element = this->inputElements[0].front();

  // an unsolved plane has no meaningful normal yet, so the line it would
  // define would be meaningless too
  if (element.plane.isNull() || !element.plane->getIsSolved()) {
    this->setIsUsed(0, element.id, false);
    this->sendMessage(std::format("The plane for line {} is not solved",
                                  line.getFeatureName()),
                      MessageTypes::eWarningMessage);
    return false;
  }

  this->setIsUsed(0, element.id, true);

  // the plane's own position and normal define the line directly - no fit,
  // no approximation, just the geometry that is already there
  Position linePosition = element.plane->getPosition();
  Direction lineDirection = element.plane->getDirection();

  line.setLine(linePosition, lineDirection);

  return true;
}
