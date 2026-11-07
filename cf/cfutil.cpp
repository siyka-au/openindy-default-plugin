#include "cfutil.h"

#include <format>
#include <stdexcept>

FeatureTypes CFUtil::featureTypesForName(std::string name) {
  if (name.compare("Circle") == 0) {
    return FeatureTypes::eCircleFeature;
  } else if (name.compare("Cone") == 0) {
    return FeatureTypes::eConeFeature;
  } else if (name.compare("Cylinder") == 0) {
    return FeatureTypes::eCylinderFeature;
  } else if (name.compare("Ellipse") == 0) {
    return FeatureTypes::eEllipseFeature;
  } else if (name.compare("Ellipsoid") == 0) {
    return FeatureTypes::eEllipsoidFeature;
  } else if (name.compare("Hyperboloid") == 0) {
    return FeatureTypes::eHyperboloidFeature;
  } else if (name.compare("Line") == 0) {
    return FeatureTypes::eLineFeature;
  } else if (name.compare("Nurbs") == 0) {
    return FeatureTypes::eNurbsFeature;
  } else if (name.compare("Paraboloid") == 0) {
    return FeatureTypes::eParaboloidFeature;
  } else if (name.compare("Plane") == 0) {
    return FeatureTypes::ePlaneFeature;
  } else if (name.compare("Point") == 0) {
    return FeatureTypes::ePointFeature;
  } else if (name.compare("PointCloud") == 0) {
    return FeatureTypes::ePointCloudFeature;
  } else if (name.compare("ScalarEntityAngle") == 0) {
    return FeatureTypes::eScalarEntityAngleFeature;
  } else if (name.compare("ScalarEntityDistance") == 0) {
    return FeatureTypes::eScalarEntityDistanceFeature;
  } else if (name.compare("ScalarEntityMeasurementSeries") == 0) {
    return FeatureTypes::eScalarEntityMeasurementSeriesFeature;
  } else if (name.compare("ScalarEntityTemperature") == 0) {
    return FeatureTypes::eScalarEntityTemperatureFeature;
  } else if (name.compare("SlottedHole") == 0) {
    return FeatureTypes::eSlottedHoleFeature;
  } else if (name.compare("Sphere") == 0) {
    return FeatureTypes::eSphereFeature;
  } else if (name.compare("Torus") == 0) {
    return FeatureTypes::eTorusFeature;
  } else if (name.compare("CoordinateSystem") == 0) {
    return FeatureTypes::eCoordinateSystemFeature;
  } else if (name.compare("Station") == 0) {
    return FeatureTypes::eStationFeature;
  } else if (name.compare("TrafoParam") == 0) {
    return FeatureTypes::eTrafoParamFeature;
  }
  throw std::logic_error(std::format("no FeatureTypes for \"{}\" found", name));
}
