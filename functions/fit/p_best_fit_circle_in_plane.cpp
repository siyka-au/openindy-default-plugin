#include "p_best_fit_circle_in_plane.h"

#include <format>
#include <vector>

/*!
 * \brief BestFitCircleInPlane::init
 */
void BestFitCircleInPlane::init() {
  // set plugin meta data
  this->metaData.name = "BestFitCircleInPlane";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{} {} {} {}", "This function calculates an adjusted circle.",
      "The points are registered into a best fit plane first and "
      "afterward a 2D circle is approximated inside the plane.",
      "You can input as many points as you want which are then used to "
      "find the best fit circle.",
      "There will be no plane feature/calculations vissible, the "
      "calculation is performed inside the function");
  this->metaData.iid = FitFunction_iidd;

  // D12/Stage 7c-ii: consolidates BestFitCircleInPlane (raw Observations)
  // and BestFitCircleInPlaneFromPoints (Points, but missing roleName
  //"default" on its primary input, so it could never actually be
  // tag-resolved either) into one tag-resolved, Point-consuming
  // declaration.
  this->neededElements.clear();
  NeededElement param1;
  param1.description =
      "Select at least three points to calculate the best fit circle.";
  param1.infinite = true;
  param1.typeOfElement = ElementTypes::ePointElement;
  param1.roleName = "default";
  this->neededElements.push_back(param1);

  NeededElement param2;
  param2.description = "Dummy point to indicate circle normal.";
  param2.infinite = true;
  param2.typeOfElement = ElementTypes::ePointElement;
  param2.roleName = "dummyPoint";
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eCircleFeature);
}

/*!
 * \brief BestFitCircleInPlane::exec
 * \param circle
 * \return
 */
std::expected<Circle, std::string> BestFitCircleInPlane::exec() {
  this->statistic.reset();
  Circle circle;
  if (!this->setUpResult(circle)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return circle;
}

/*!
 * \brief BestFitCircleInPlane::setUpResult
 * \param circle
 * \return
 */
bool BestFitCircleInPlane::setUpResult(Circle& circle) {
  if (!this->inputElements.contains(0) || this->inputElements[0].empty()) {
    this->sendMessage(
        std::format("No points tagged for circle {}", circle.getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

  std::vector<IdPoint> usablePoints;
  std::vector<IdPoint> points;
  {
    std::vector<oi::Guarded<Point> > allUsablePoints;
    std::vector<oi::Guarded<Point> > inputPoints;
    for (const InputElement& element : this->inputElements[0]) {
      if (element.point.isNull() || !element.point->getIsSolved()) {
        this->setIsUsed(0, element.id, false);
        continue;
      }
      allUsablePoints.push_back(element.point);
      this->setIsUsed(0, element.id, element.shouldBeUsed);
      if (element.shouldBeUsed) {
        inputPoints.push_back(element.point);
      }
    }

    if (inputPoints.size() < 3) {
      this->sendMessage(
          std::format("Not enough valid points to fit the circle {}",
                      circle.getFeatureName()),
          MessageTypes::eWarningMessage);
      return false;
    }

    for (const oi::Guarded<Point>& p : allUsablePoints) {
      IdPoint point;
      point.id = p->getId();
      point.xyz = p->getPosition().getVectorH();
      usablePoints.push_back(point);
    }
    for (const oi::Guarded<Point>& p : inputPoints) {
      IdPoint point;
      point.id = p->getId();
      point.xyz = p->getPosition().getVectorH();
      points.push_back(point);
    }
  }

  return bestFitCircleInPlane(this, circle, points, usablePoints);
}
