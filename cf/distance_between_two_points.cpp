#include "distance_between_two_points.h"

#include <math.h>

void DistanceBetweenTwoPoints::init() {
  this->metaData.name = "DistanceBetweenTwoPoints";
  this->applicableFor.push_back(FeatureTypes::eScalarEntityDistanceFeature);
}

std::expected<ScalarEntityDistance, std::string>
DistanceBetweenTwoPoints::exec() {
  if (this->inputElements[0].empty()) {
    this->sendMessage("inputElements[0] is empty",
                      MessageTypes::eWarningMessage);
    return std::unexpected(
        std::string("DistanceBetweenTwoPoints: inputElements[0] is empty"));
  }
  if (this->inputElements[1].empty()) {
    this->sendMessage("inputElements[1] is empty",
                      MessageTypes::eWarningMessage);
    return std::unexpected(
        std::string("DistanceBetweenTwoPoints: inputElements[1] is empty"));
  }
  OiVec p1 = this->inputElements[0].front().geometry->getPosition().getVector();
  OiVec p2 = this->inputElements[1].front().geometry->getPosition().getVector();
  OiVec v = p2 - p1;
  double dot = NAN;
  OiVec::dot(dot, v, v);
  ScalarEntityDistance distance;
  distance.setDistance(std::sqrt(dot));

  return distance;
}
