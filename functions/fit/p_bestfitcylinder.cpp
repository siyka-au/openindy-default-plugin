#include "p_bestfitcylinder.h"

#include <vector>

/*!
 * \brief BestFitCylinder::init
 */
void BestFitCylinder::init() {
  // set plugin meta data
  this->metaData.name = "BestFitCylinder";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description =
      QString("%1 %2")
          .arg("This function calculates an adjusted cylinder.")
          .arg(
              "You can input as many points as you want which are then used to "
              "find the best fit cylinder.");
  this->metaData.iid = FitFunction_iidd;

  // D12/Stage 7c-ii: cylinder is not a special case - ePointElement with
  // a minimum of 5 (2 DOF axis direction + 2 DOF point-on-axis + 1 DOF
  // radius), the same shape as plane's 3. Consolidates BestFitCylinder
  //(raw Observations) and BestFitCylinderFromPoints (declared
  // eObservationElement despite its name - setUpResult actually read
  // element.point, so it was unreachable through ordinary binding; see
  // domain-model.md/plan Stage 7c-ii notes) into one tag-resolved,
  // Point-consuming declaration.
  this->neededElements.clear();
  NeededElement param1;
  param1.description =
      "Select at least five points to calculate the best fit cylinder.";
  param1.infinite = true;
  param1.typeOfElement = ElementTypes::ePointElement;
  param1.roleName = "default";
  this->neededElements.push_back(param1);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eCylinderFeature);

  this->stringParameters.emplace("approximation", "first two points");
  this->stringParameters.emplace("approximation", "guess axis");

  this->scalarInputParams.isValid = true;
  this->scalarInputParams.stringParameter.insert_or_assign(
      "approximation", "first two points");  // default
}

/*!
 * \brief BestFitCylinder::exec
 * \param cylinder
 * \return
 */
std::expected<Cylinder, QString> BestFitCylinder::exec() {
  this->statistic.reset();
  Cylinder cylinder;
  if (!BestFitCylinder::setUpResult(cylinder)) {
    return std::unexpected(QString("%1 failed").arg(this->getMetaData().name));
  }
  return cylinder;
}

/*!
 * \brief BestFitCylinder::setUpResult
 * \param cylinder
 * \return
 */
bool BestFitCylinder::setUpResult(Cylinder& cylinder) {
  if (!this->inputElements.contains(0) || this->inputElements[0].empty()) {
    emit this->sendMessage(QString("No points tagged for cylinder %1")
                               .arg(cylinder.getFeatureName()),
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

    // five is the geometric minimum (2 DOF axis direction + 2 DOF
    // point-on-axis + 1 DOF radius) - a hint, not a gate: the real
    // validation is whether the fit below actually converges.
    if (inputPoints.size() < 5) {
      emit this->sendMessage(
          QString("Not enough valid points to fit the cylinder %1")
              .arg(cylinder.getFeatureName()),
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

  return bestFitCylinder(this, cylinder, points, usablePoints);
}

/**
 * @brief BestFitCylinderAppxDirection::init
 */
void BestFitCylinderAppxDirection::init() {
  // set plugin meta data
  this->metaData.name = "BestFitCylinderAppxDirection";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "esc";
  this->metaData.description =
      QString("%1 %2 %3")
          .arg("This function calculates an adjusted cylinder.")
          .arg(
              "You can input as many points as you want which are then used to "
              "find the best fit cylinder.")
          .arg(
              "Additionally you should input a direction for approximation, "
              "this can be done by all vector geometries");
  this->metaData.iid = FitFunction_iidd;

  // set needed elements
  this->neededElements.clear();
  NeededElement param1;
  param1.description =
      "Select at least five points to calculate the best fit cylinder.";
  param1.infinite = true;
  param1.typeOfElement = ElementTypes::ePointElement;
  param1.roleName = "default";
  this->neededElements.push_back(param1);

  NeededElement param2;
  param2.description = "approximation direction.";
  param2.infinite = true;
  param2.typeOfElement = ElementTypes::eDirectionElement;
  this->neededElements.push_back(param2);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eCylinderFeature);

  this->scalarInputParams.isValid = true;
  this->scalarInputParams.stringParameter.insert_or_assign(
      "approximation",
      "direction");  // default
}

/**
 * @brief BestFitCylinderAppxDummyPoint::init
 */
void BestFitCylinderAppxDummyPoint::init() {
  // set plugin meta data
  this->metaData.name = "BestFitCylinderAppxDummyPoint";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "esc";
  this->metaData.description =
      QString("%1 %2 %3")
          .arg("This function calculates an adjusted cylinder.")
          .arg(
              "You can input as many points as you want which are then used to "
              "find the best fit cylinder.")
          .arg(
              "Additionally you should input dummy points for approximation, "
              "this can be done by all point geometries");
  this->metaData.iid = FitFunction_iidd;

  // set needed elements
  this->neededElements.clear();
  NeededElement param1;
  param1.description =
      "Select at least five points to calculate the best fit cylinder.";
  param1.infinite = true;
  param1.typeOfElement = ElementTypes::ePointElement;
  param1.roleName = "default";
  this->neededElements.push_back(param1);

  NeededElement param3;
  param3.description = "Dummy points to indicate cylinder normal.";
  param3.infinite = true;
  param3.typeOfElement = ElementTypes::ePointElement;
  param3.roleName = "dummyPoint";
  this->neededElements.push_back(param3);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eCylinderFeature);

  this->scalarInputParams.isValid = true;
  this->scalarInputParams.stringParameter.insert_or_assign(
      "approximation", "first two dummy points");
}
