#include "p_intersectlineline.h"

#include <math.h>

#include <format>

#include "containers.h"

/*!
 * \brief IntersectLineLine::init
 */
void IntersectLineLine::init() {
  // set plugin meta data
  this->metaData.name = "IntersectLineLine";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "esc";
  this->metaData.description = std::format(
      "{}", "This function calculates the intersection of a line with a line.");
  this->metaData.iid = ConstructFunction_iidd;

  // set needed elements
  NeededElement param1;
  param1.description = "Select a line to calculate the intersection.";
  param1.infinite = false;
  param1.typeOfElement = ElementTypes::eLineElement;
  this->neededElements.push_back(param1);

  NeededElement param2;
  param2.description = "Select a line to calculate the intersection.";
  param2.infinite = false;
  param2.typeOfElement = ElementTypes::eLineElement;
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::ePointFeature);

  this->stringParameters.emplace("intersection", "at first line");
  this->stringParameters.emplace("intersection", "at second line");
  this->stringParameters.emplace("intersection", "midpoint");

  this->scalarInputParams.isValid = true;
  this->scalarInputParams.stringParameter.insert_or_assign(
      "intersection",
      "midpoint");  // default
}

IntersectionTypes IntersectLineLine::getIntersectionType() {
  if (this->scalarInputParams.stringParameter.contains("intersection")) {
    if (oi::value(this->scalarInputParams.stringParameter, "intersection")
            .compare("at first line") == 0) {
      return IntersectionTypes::eAtFirstLine;
    } else if (oi::value(this->scalarInputParams.stringParameter,
                         "intersection")
                   .compare("at second line") == 0) {
      return IntersectionTypes::eAtSecondLine;
    } else if (oi::value(this->scalarInputParams.stringParameter,
                         "intersection")
                   .compare("midpoint") == 0) {
      return IntersectionTypes::eMidPoint;
    }
  }

  return IntersectionTypes::eAtFirstLine;  // default
}

/*!
 * \brief IntersectLineLine::exec
 * \param point
 * \return
 */
std::expected<Point, std::string> IntersectLineLine::exec() {
  Point point;
  if (!this->setUpResult(point)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return point;
}

/*!
 * \brief IntersectLineLine::setUpResult
 * \param point
 * \return
 */
bool IntersectLineLine::setUpResult(Point& point) {
  // get and check line and plane that shall be intersected
  if (!this->inputElements.contains(0) || this->inputElements[0].size() != 1 ||
      !this->inputElements.contains(1) || this->inputElements[1].size() != 1) {
    return false;
  }
  oi::Guarded<Line> line1 = this->inputElements[0].at(0).line;
  oi::Guarded<Line> line2 = this->inputElements[1].at(0).line;
  if (line1.isNull() || line2.isNull() || !line1->getIsSolved() ||
      !line2->getIsSolved()) {
    return false;
  }

  if (this->isParallel(line1, line2)) {
    this->sendMessage(
        std::format("line {} and line {} are parallel", line1->getFeatureName(),
                    line2->getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

  // perform intersection
  OiVec a = line1->getPosition().getVector();
  OiVec b = line2->getPosition().getVector();
  OiVec v = line1->getDirection().getVector();
  OiVec w = line2->getDirection().getVector();

  OiVec cvw, n1, n2;
  OiVec::cross(cvw, v, w);
  OiVec::cross(n1, v, cvw);
  OiVec::cross(n2, w, cvw);

  double dan1 = NAN;
  double dbn1 = NAN;
  double dwn1 = NAN;
  OiVec::dot(dan1, a, n1);
  OiVec::dot(dbn1, b, n1);
  OiVec::dot(dwn1, w, n1);

  double dbn2 = NAN;
  double dan2 = NAN;
  double dvn2 = NAN;
  OiVec::dot(dbn2, b, n2);
  OiVec::dot(dan2, a, n2);
  OiVec::dot(dvn2, v, n2);

  OiVec fline1 = (dbn2 - dan2) / dvn2 * v + a;
  OiVec fline2 = (dan1 - dbn1) / dwn1 * w + b;

  OiVec s;
  switch (getIntersectionType()) {
    case IntersectionTypes::eAtSecondLine:
      s = fline2;
      break;

    case IntersectionTypes::eMidPoint:
      s = (fline1 + fline2) / 2.;
      break;

    case IntersectionTypes::eAtFirstLine:  // fall though
    default:
      s = fline1;
  }

  // set result
  Position section;
  section.setVector(s);
  point.setPoint(section);

  //  show / log distance between fg & fh
  OiVec delta = fline1 - fline2;
  double dot = NAN;
  OiVec::dot(dot, delta, delta);
  point.setDistance(sqrt(dot));

  return true;
}

/*!
 * \brief IntersectLineLine::isParallel
 * \param line
 * \param plane
 * \return
 */
bool IntersectLineLine::isParallel(const oi::Guarded<Line>& line1,
                                   const oi::Guarded<Line>& line2) {
  // scalar product of the lines's direction vector
  double dot = 0.0;
  OiVec::dot(dot, line1->getDirection().getVector(),
             line2->getDirection().getVector());
  return dot > 0.999;
}
