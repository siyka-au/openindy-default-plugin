#include "p_nominalpoint.h"

#include <format>

#include "containers.h"

/*!
 * \brief NominalPoint::init
 */
void NominalPoint::init() {
  // set plugin meta data
  this->metaData.name = "NominalPoint";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "OpenIndy";
  this->metaData.description = std::format(
      "{}",
      "Defines a point from hand-entered x/y/z coordinates. Produces a "
      "nominal: user-editable, no measurement involved.");
  this->metaData.iid = ConstructFunction_iidd;

  // a nominal function takes scalar parameters only - no needed elements.
  // This absence is exactly what Geometry::getOwnerDescription reads to
  // know this result is not owned by a fit.
  this->neededElements.clear();

  // set applicable for
  this->applicableFor.push_back(FeatureTypes::ePointFeature);

  // scalar input parameters, in the job's default unit (metres)
  this->doubleParameters.insert_or_assign("x [m]", 0.0);
  this->doubleParameters.insert_or_assign("y [m]", 0.0);
  this->doubleParameters.insert_or_assign("z [m]", 0.0);
}

/*!
 * \brief NominalPoint::exec
 * \param point
 * \return
 */
std::expected<Point, std::string> NominalPoint::exec() {
  Point point;

  // user-entered values, falling back to the declared defaults - same
  // pattern as ObjectTransformation::ChangeRadius
  double x = oi::value(this->doubleParameters, "x [m]", 0.0);
  double y = oi::value(this->doubleParameters, "y [m]", 0.0);
  double z = oi::value(this->doubleParameters, "z [m]", 0.0);
  if (this->scalarInputParams.doubleParameter.contains("x [m]")) {
    x = oi::value(this->scalarInputParams.doubleParameter, "x [m]");
  }
  if (this->scalarInputParams.doubleParameter.contains("y [m]")) {
    y = oi::value(this->scalarInputParams.doubleParameter, "y [m]");
  }
  if (this->scalarInputParams.doubleParameter.contains("z [m]")) {
    z = oi::value(this->scalarInputParams.doubleParameter, "z [m]");
  }

  point.setPoint(Position(x, y, z));

  // no uncertainty - this is a specification, not a measurement
  //(tolerance is deferred and has nowhere to attach yet)
  return point;
}
