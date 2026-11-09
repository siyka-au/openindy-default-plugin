#include "p_best_fit_line.h"

#include <math.h>

#include <format>
#include <utility>
#include <vector>

namespace {
// A 3D line has 4 parameters and two effective errors per point (the two
// coordinates perpendicular to the line), so the redundancy is 2n - 4.
constexpr int kLineFittedParameters = 4;
}  // namespace

void BestFitLine::init() {
  // set plugin meta data
  this->metaData.name = "BestFitLine";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = std::format(
      "{} {}", "This function calculates an adjusted line.",
      "You can input as many points as you want which are then used to "
      "find the best fit 3D line.");
  this->metaData.iid = FitFunction_iidd;

  // D12/Stage 7c-ii: consolidates BestFitLine (raw Observations) and
  // LineFromPoints (Points, not tag-resolved) into one tag-resolved,
  // Point-consuming declaration - see p_best_fit_plane.cpp for the pattern.
  this->neededElements.clear();
  NeededElement param1;
  param1.description =
      "Select at least two points to calculate the best fit line.";
  param1.infinite = true;
  param1.typeOfElement = ElementTypes::ePointElement;
  param1.roleName = "default";
  this->neededElements.push_back(param1);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eLineFeature);
}

/*!
 * \brief BestFitLine::exec
 * \param line
 * \return
 */
std::expected<Line, std::string> BestFitLine::exec() {
  this->statistic.reset();
  Line line;
  if (!this->setUpResult(line)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return line;
}

/*!
 * \brief BestFitLine::setUpResult
 * Set up result and statistic for type plane
 * \param p
 */
bool BestFitLine::setUpResult(Line& line) {
  if (!this->inputElements.contains(0) || this->inputElements[0].empty()) {
    this->sendMessage(
        std::format("No points tagged for line {}", line.getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }

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
  if (inputPoints.size() < 2) {
    this->sendMessage(std::format("Not enough valid points to fit the line {}",
                                  line.getFeatureName()),
                      MessageTypes::eWarningMessage);
    return false;
  }

  // centroid
  OiVec centroid(3);
  for (const oi::Guarded<Point>& point : inputPoints) {
    centroid = centroid + point->getPosition().getVector();
  }
  centroid = centroid * (1.0 / static_cast<double>(inputPoints.size()));

  // principle component analysis
  OiMat a(inputPoints.size(), 3);
  for (int i = 0; std::cmp_less(i, inputPoints.size()); i++) {
    const OiVec pos = inputPoints.at(i)->getPosition().getVector();
    a.setAt(i, 0, pos.getAt(0) - centroid.getAt(0));
    a.setAt(i, 1, pos.getAt(1) - centroid.getAt(1));
    a.setAt(i, 2, pos.getAt(2) - centroid.getAt(2));
  }
  OiMat ata = a.t() * a;
  OiMat u(3, 3);
  OiVec d(3);
  OiMat v(3, 3);
  ata.svd(u, d, v);

  // get largest eigenvector which is r vector and v^T * v as sum of the 2
  // smaller eigenvectors
  int eigenIndex = -1;
  double eVal = 0.0;
  double vtv = 0.0;
  for (int i = 0; i < d.getSize(); i++) {
    if (d.getAt(i) > eVal || i == 0) {
      eVal = d.getAt(i);
      eigenIndex = i;
    } else {
      vtv += d.getAt(i);
    }
  }
  OiVec r(3);
  u.getCol(r, eigenIndex);
  r.normalize();

  // check that the orientation of the line is from first to second point
  OiVec pos1 = inputPoints.at(0)->getPosition().getVector();
  OiVec pos2 = inputPoints.at(1)->getPosition().getVector();
  OiVec direction = pos2 - pos1;
  direction.normalize();
  rectifyNormalToDirection(r, direction);

  // calculate display residuals for each usable point
  for (const oi::Guarded<Point>& point : allUsablePoints) {
    double distance = 0.0;
    OiVec v_line(3);
    const OiVec pos = point->getPosition().getVector();
    // calculate perpendicular
    v_line.setAt(0, pos.getAt(0) - centroid.getAt(0));
    v_line.setAt(1, pos.getAt(1) - centroid.getAt(1));
    v_line.setAt(2, pos.getAt(2) - centroid.getAt(2));
    OiVec::dot(distance, r, v_line);
    v_line = centroid + distance * r;

    // calculate residual vector
    v_line.setAt(0, pos.getAt(0) - v_line.getAt(0));
    v_line.setAt(1, pos.getAt(1) - v_line.getAt(1));
    v_line.setAt(2, pos.getAt(2) - v_line.getAt(2));

    // set up display residual
    double dot = NAN;
    OiVec::dot(dot, v_line, v_line);
    addDisplayResidual(point->getId(), v_line.getAt(0), v_line.getAt(1),
                       v_line.getAt(2), std::sqrt(dot));
  }

  // set result
  Position linePosition;
  linePosition.setVector(centroid);
  Direction lineDirection;
  lineDirection.setVector(r);
  line.setLine(linePosition, lineDirection);

  // set statistic
  this->statistic.setIsValid(true);
  const int numPoints = static_cast<int>(inputPoints.size());
  double stdv = 0.0;
  if (2 * numPoints > kLineFittedParameters) {
    stdv = std::sqrt(vtv / (2 * numPoints - kLineFittedParameters));
  } else {
    this->sendMessage(
        "The standard deviation of the line is undefined without redundant "
        "points (more than 2 are needed); it is reported as 0",
        MessageTypes::eWarningMessage);
  }
  this->statistic.setStdev(stdv);
  line.setStatistic(this->statistic);

  return true;
}
