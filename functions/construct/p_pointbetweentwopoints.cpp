#include "p_pointbetweentwopoints.h"

#include <format>

/*!
 * \brief PointBetweenTwoPoints::init
 */
void PointBetweenTwoPoints::init() {
  // set plugin meta data
  this->metaData.name = "PointBetweenTwoPoints";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{} {}", "This function calculates the point between two points.",
      "Therefore you have to input those two points.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select first point.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::ePointElement;
  NeededElement param2;
  param2.description = "Select second point.";
  param2.infinite = false;
  param2.typeOfElement = ElementTypes::ePointElement;
  this->neededElements.push_back(param1);
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePointFeature);
}

/*!
 * \brief PointBetweenTwoPoints::exec
 * \param point
 * \return
 */
std::expected<Point, std::string> PointBetweenTwoPoints::exec() {
  Point point;
  if (!this->setUpResult(point)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return point;
}

/*!
 * \brief PointBetweenTwoPoints::setUpResult
 * \param point
 * \return
 */
bool PointBetweenTwoPoints::setUpResult(Point& point) {
  // get and check input points
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1 ||
      !this->inputElements.contains(1) || this->inputElements.size() != 1) {
    return false;
  }

  // check if input elements are equal
  if (this->inputElements[0].front().point->getId() ==
      this->inputElements[1].front().point->getId()) {
    this->sendMessage(std::string("Input geometries are equal."),
                      MessageTypes::eWarningMessage);
    return false;
  }

  oi::Guarded<Point> point1 = this->inputElements[0].at(0).point;
  oi::Guarded<Point> point2 = this->inputElements[1].at(0).point;
  if (point1.isNull() || point2.isNull() || !point1->getIsSolved() ||
      !point2->getIsSolved()) {
    return false;
  }

  // calculate midpoint
  OiVec center =
      (point1->getPosition().getVector() + point2->getPosition().getVector()) *
      0.5;

  // set result
  Position position;
  position.setVector(center);
  point.setPoint(position);

  // set statistic
  this->statistic.setIsValid(true);
  this->statistic.setStdev(
      (point1->getStatistic().getStdev() + point2->getStatistic().getStdev()) *
      0.5);
  point.setStatistic(this->statistic);

  return true;
}
