#include "p_factory.h"

#include <QtCore>
#include <format>
#include <memory>
#include <vector>

#include "metaInfo.h"

using namespace oi;

OiTemplatePlugin::OiTemplatePlugin() {}

/*!
 * \brief OiTemplatePlugin::createSensors
 * Returns a list with all sensors
 * \return
 */
std::vector<std::unique_ptr<Sensor>> OiTemplatePlugin::createSensors() {
  std::vector<std::unique_ptr<Sensor>> resultSet;
  resultSet.push_back(std::make_unique<PseudoTracker>());
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createFunctions
 * Returns a list with all functions
 * \return
 */
std::vector<std::unique_ptr<Function>> OiTemplatePlugin::createFunctions() {
  std::vector<std::unique_ptr<Function>> resultSet;

  resultSet.push_back(std::make_unique<NominalPoint>());

  resultSet.push_back(std::make_unique<BestFitPoint>());
  resultSet.push_back(std::make_unique<BestFitLine>());
  resultSet.push_back(std::make_unique<BestFitPlane>());
  resultSet.push_back(std::make_unique<BestFitCircle>());
  resultSet.push_back(std::make_unique<PointFromPoints>());
  resultSet.push_back(std::make_unique<PlaneFromPlane>());
  resultSet.push_back(std::make_unique<LineFromPlaneNormal>());
  resultSet.push_back(std::make_unique<PointFromPlaneOrigin>());
  resultSet.push_back(std::make_unique<AverageFromPoints>());
  resultSet.push_back(std::make_unique<PointBetweenTwoPoints>());
  resultSet.push_back(std::make_unique<IntersectLineLine>());
  resultSet.push_back(std::make_unique<IntersectLinePlane>());
  resultSet.push_back(std::make_unique<IntersectPlanePlane>());
  resultSet.push_back(std::make_unique<ChangeRadius>());
  resultSet.push_back(std::make_unique<TranslateByValue>());
  resultSet.push_back(std::make_unique<Register>());
  resultSet.push_back(std::make_unique<BestFitSphere>());
  resultSet.push_back(std::make_unique<BestFitCircleInPlane>());
  resultSet.push_back(std::make_unique<RectifyToPoint>());
  resultSet.push_back(std::make_unique<RectifyToVector>());
  resultSet.push_back(std::make_unique<Shift>());
  resultSet.push_back(std::make_unique<BestFitCylinder>());
  resultSet.push_back(std::make_unique<BestFitCylinderAppxDirection>());
  resultSet.push_back(std::make_unique<BestFitCylinderAppxDummyPoint>());
  resultSet.push_back(std::make_unique<Helmert7Param>());
  resultSet.push_back(std::make_unique<Helmert6Param>());

  FunctionConfigParser parser;
  for (ConfiguredFunctionConfig config : parser.readConfigFromJson()) {
    if (config.version == 2) {
      // this instance contains only function name!
      resultSet.push_back(std::make_unique<ConfiguredFunction>(
          config, std::vector<std::unique_ptr<Function>>()));
    }
  }

  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createBundleAdjustments
 * Returns a list with all network adjustments
 * \return
 */
std::vector<std::unique_ptr<BundleAdjustment>>
OiTemplatePlugin::createBundleAdjustments() {
  std::vector<std::unique_ptr<BundleAdjustment>> resultSet;
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSimulations
 * Returns a list with all simulations
 * \return
 */
std::vector<std::unique_ptr<SimulationModel>>
OiTemplatePlugin::createSimulations() {
  std::vector<std::unique_ptr<SimulationModel>> resultSet;
  resultSet.push_back(std::make_unique<SimplePolarMeasurement>());
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSimpleAsciiExchanges
 * \return
 */
std::vector<std::unique_ptr<ExchangeSimpleAscii>>
OiTemplatePlugin::createSimpleAsciiExchanges() {
  std::vector<std::unique_ptr<ExchangeSimpleAscii>> resultSet;
  resultSet.push_back(std::make_unique<OiExchangeAscii>());
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createDefinedFormatExchanges
 * \return
 */
std::vector<std::unique_ptr<ExchangeDefinedFormat>>
OiTemplatePlugin::createDefinedFormatExchanges() {
  std::vector<std::unique_ptr<ExchangeDefinedFormat>> resultSet;
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSensor
 * Returns the sensor with the specified name
 * \param name
 * \return
 */
std::unique_ptr<Sensor> OiTemplatePlugin::createSensor(
    const std::string& name) {
  std::unique_ptr<Sensor> result;
  if (name.compare("PseudoTracker") == 0) {
    result = std::make_unique<PseudoTracker>();
  }
  return result;
}

/*!
 * \brief OiTemplatePlugin::createFunction
 * Returns the function with the specified name
 * \param name
 * \return
 */
std::unique_ptr<Function> OiTemplatePlugin::createFunction(
    const std::string& name) {
  std::unique_ptr<Function> result;
  if (name.compare("NominalPoint") == 0) {
    result = std::make_unique<NominalPoint>();
  } else if (name.compare("BestFitPoint") == 0) {
    result = std::make_unique<BestFitPoint>();
  } else if (name.compare("BestFitLine") == 0) {
    result = std::make_unique<BestFitLine>();
  } else if (name.compare("BestFitPlane") == 0) {
    result = std::make_unique<BestFitPlane>();
  } else if (name.compare("PointFromPoints") == 0) {
    result = std::make_unique<PointFromPoints>();
  } else if (name.compare("PlaneFromPlane") == 0) {
    result = std::make_unique<PlaneFromPlane>();
  } else if (name.compare("LineFromPlaneNormal") == 0) {
    result = std::make_unique<LineFromPlaneNormal>();
  } else if (name.compare("PointFromPlaneOrigin") == 0) {
    result = std::make_unique<PointFromPlaneOrigin>();
  } else if (name.compare("AverageFromPoints") == 0) {
    result = std::make_unique<AverageFromPoints>();
  } else if (name.compare("PointBetweenTwoPoints") == 0) {
    result = std::make_unique<PointBetweenTwoPoints>();
  } else if (name.compare("IntersectLineLine") == 0) {
    result = std::make_unique<IntersectLineLine>();
  } else if (name.compare("IntersectLinePlane") == 0) {
    result = std::make_unique<IntersectLinePlane>();
  } else if (name.compare("IntersectPlanePlane") == 0) {
    result = std::make_unique<IntersectPlanePlane>();
  } else if (name.compare("ChangeRadius") == 0) {
    result = std::make_unique<ChangeRadius>();
  } else if (name.compare("TranslateByValue") == 0) {
    result = std::make_unique<TranslateByValue>();
  } else if (name.compare("Register") == 0) {
    result = std::make_unique<Register>();
  } else if (name.compare("HelmertTransformation") == 0) {
    result = std::make_unique<Helmert7Param>();
  } else if (name.compare("Helmert6Param") == 0) {
    result = std::make_unique<Helmert6Param>();
  } else if (name.compare("BestFitCircle") == 0) {
    result = std::make_unique<BestFitCircle>();
  } else if (name.compare("BestFitCircleInPlane") == 0) {
    result = std::make_unique<BestFitCircleInPlane>();
  } else if (name.compare("BestFitSphere") == 0) {
    result = std::make_unique<BestFitSphere>();
  } else if (name.compare("RectifyToPoint") == 0) {
    result = std::make_unique<RectifyToPoint>();
  } else if (name.compare("Shift") == 0) {
    result = std::make_unique<Shift>();
  } else if (name.compare("RectifyToVector") == 0) {
    return std::make_unique<RectifyToVector>();
  } else if (name.compare("BestFitCylinder") == 0) {
    return std::make_unique<BestFitCylinder>();
  } else if (name.compare("BestFitCylinderAppxDirection") == 0) {
    return std::make_unique<BestFitCylinderAppxDirection>();
  } else if (name.compare("BestFitCylinderAppxDummyPoint") == 0) {
    return std::make_unique<BestFitCylinderAppxDummyPoint>();
  } else {
    return createFunctionFromConfig(name);
  }
  return result;
}

std::unique_ptr<Function> OiTemplatePlugin::createFunctionFromConfig(
    const std::string& functionName) {
  // internal functions
  if (functionName.compare("DistanceBetweenTwoPoints") == 0) {
    return std::make_unique<DistanceBetweenTwoPoints>();
  } else if (functionName.compare("XYFilter") == 0) {
    return std::make_unique<XYZFilter>("XYFilter", true, true,
                                       false);  // keep Z
  } else if (functionName.compare("XZFilter") == 0) {
    return std::make_unique<XYZFilter>("XZFilter", true, false,
                                       true);  // keep Y
  } else if (functionName.compare("YZFilter") == 0) {
    return std::make_unique<XYZFilter>("YZFilter", false, true,
                                       true);  // keep X
  }

  std::unique_ptr<Function> function;

  FunctionConfigParser parser;
  for (ConfiguredFunctionConfig config : parser.readConfigFromJson()) {
    if (config.name.compare(functionName) == 0) {
      std::vector<std::unique_ptr<Function>> functions;
      for (std::string name : config.getFunctionNames()) {
        std::unique_ptr<Function> f = createFunction(name);
        if (!f) {
          throw logic_error(std::format("no function found: {}", name));
        }
        f->init();
        functions.push_back(std::move(f));
      }

      if (config.version == 2) {
        function =
            std::make_unique<ConfiguredFunction>(config, std::move(functions));
        function->init();
        break;
      }
    }

    if (function) {
      return function;
    }
  }

  return function;
}

/*!
 * \brief OiTemplatePlugin::createBundleAdjustment
 * Returns the network adjustment with the specified name
 * \param name
 * \return
 */
std::unique_ptr<BundleAdjustment> OiTemplatePlugin::createBundleAdjustment(
    const std::string& name) {
  std::unique_ptr<BundleAdjustment> result;
  return result;
}

/*!
 * \brief OiTemplatePlugin::createSimulation
 * Returns the simulation with the specified name
 * \param name
 * \return
 */
std::unique_ptr<SimulationModel> OiTemplatePlugin::createSimulation(
    const std::string& name) {
  std::unique_ptr<SimulationModel> result;

  if (name.compare("SimplePolarMeasurement") == 0) {
    result = std::make_unique<SimplePolarMeasurement>();
  }

  return result;
}

/*!
 * \brief OiTemplatePlugin::createSimpleAsciiExchange
 * \param name
 * \return
 */
std::unique_ptr<ExchangeSimpleAscii>
OiTemplatePlugin::createSimpleAsciiExchange(const std::string& name) {
  std::unique_ptr<ExchangeSimpleAscii> result;

  if (name.compare("OiExchangeAscii") == 0) {
    result = std::make_unique<OiExchangeAscii>();
  }

  return result;
}

/*!
 * \brief OiTemplatePlugin::createDefinedFormatExchange
 * \param name
 * \return
 */
std::unique_ptr<ExchangeDefinedFormat>
OiTemplatePlugin::createDefinedFormatExchange(const std::string& name) {
  std::unique_ptr<ExchangeDefinedFormat> result;
  return result;
}

OI_DECLARE_PLUGIN_WITH_METADATA(OiTemplatePlugin, kOiPluginMetadataJson)
