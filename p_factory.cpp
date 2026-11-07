#include "p_factory.h"

using namespace oi;

OiTemplatePlugin::OiTemplatePlugin(QObject* parent) : QObject(parent) {}

/*!
 * \brief OiTemplatePlugin::createSensors
 * Returns a list with all sensors
 * \return
 */
QList<oi::Guarded<Sensor> > OiTemplatePlugin::createSensors() {
  QList<oi::Guarded<Sensor> > resultSet;
  oi::Guarded<Sensor> pTracker = new PseudoTracker();
  resultSet.append(pTracker);
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createFunctions
 * Returns a list with all functions
 * \return
 */
QList<oi::Guarded<Function> > OiTemplatePlugin::createFunctions() {
  QList<oi::Guarded<Function> > resultSet;

  resultSet.append(new NominalPoint());

  resultSet.append(new BestFitPoint());
  resultSet.append(new BestFitLine());
  resultSet.append(new BestFitPlane());
  resultSet.append(new BestFitCircle());
  resultSet.append(new PointFromPoints());
  resultSet.append(new PlaneFromPlane());
  resultSet.append(new LineFromPlaneNormal());
  resultSet.append(new PointFromPlaneOrigin());
  resultSet.append(new AverageFromPoints());
  resultSet.append(new PointBetweenTwoPoints());
  resultSet.append(new IntersectLineLine());
  resultSet.append(new IntersectLinePlane());
  resultSet.append(new IntersectPlanePlane());
  resultSet.append(new ChangeRadius());
  resultSet.append(new TranslateByValue());
  resultSet.append(new Register());
  resultSet.append(new BestFitSphere());
  resultSet.append(new BestFitCircleInPlane());
  resultSet.append(new RectifyToPoint());
  resultSet.append(new RectifyToVector());
  resultSet.append(new Shift());
  resultSet.append(new BestFitCylinder());
  resultSet.append(new BestFitCylinderAppxDirection());
  resultSet.append(new BestFitCylinderAppxDummyPoint());
  resultSet.append(new Helmert7Param());
  resultSet.append(new Helmert6Param());

  FunctionConfigParser parser;
  foreach (ConfiguredFunctionConfig config, parser.readConfigFromJson()) {
    QList<oi::Guarded<Function> > functions;
    if (config.version == 2) {
      resultSet.append(new ConfiguredFunction(
          config, functions));  // this instance contains only function name!
    }
  }

  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createBundleAdjustments
 * Returns a list with all network adjustments
 * \return
 */
QList<oi::Guarded<BundleAdjustment> >
OiTemplatePlugin::createBundleAdjustments() {
  QList<oi::Guarded<BundleAdjustment> > resultSet;
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSimulations
 * Returns a list with all simulations
 * \return
 */
QList<oi::Guarded<SimulationModel> > OiTemplatePlugin::createSimulations() {
  QList<oi::Guarded<SimulationModel> > resultSet;
  resultSet.append(new SimplePolarMeasurement());
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSimpleAsciiExchanges
 * \return
 */
QList<oi::Guarded<ExchangeSimpleAscii> >
OiTemplatePlugin::createSimpleAsciiExchanges() {
  QList<oi::Guarded<ExchangeSimpleAscii> > resultSet;
  resultSet.append(new OiExchangeAscii());
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createDefinedFormatExchanges
 * \return
 */
QList<oi::Guarded<ExchangeDefinedFormat> >
OiTemplatePlugin::createDefinedFormatExchanges() {
  QList<oi::Guarded<ExchangeDefinedFormat> > resultSet;
  return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSensor
 * Returns the sensor with the specified name
 * \param name
 * \return
 */
oi::Guarded<Sensor> OiTemplatePlugin::createSensor(const QString& name) {
  oi::Guarded<Sensor> result(nullptr);
  if (name.compare("PseudoTracker") == 0) {
    result = new PseudoTracker();
  }
  return result;
}

/*!
 * \brief OiTemplatePlugin::createFunction
 * Returns the function with the specified name
 * \param name
 * \return
 */
oi::Guarded<Function> OiTemplatePlugin::createFunction(const QString& name) {
  oi::Guarded<Function> result(nullptr);
  if (name.compare("NominalPoint") == 0) {
    result = new NominalPoint();
  } else if (name.compare("BestFitPoint") == 0) {
    result = new BestFitPoint();
  } else if (name.compare("BestFitLine") == 0) {
    result = new BestFitLine();
  } else if (name.compare("BestFitPlane") == 0) {
    result = new BestFitPlane();
  } else if (name.compare("PointFromPoints") == 0) {
    result = new PointFromPoints();
  } else if (name.compare("PlaneFromPlane") == 0) {
    result = new PlaneFromPlane();
  } else if (name.compare("LineFromPlaneNormal") == 0) {
    result = new LineFromPlaneNormal();
  } else if (name.compare("PointFromPlaneOrigin") == 0) {
    result = new PointFromPlaneOrigin();
  } else if (name.compare("AverageFromPoints") == 0) {
    result = new AverageFromPoints();
  } else if (name.compare("PointBetweenTwoPoints") == 0) {
    result = new PointBetweenTwoPoints();
  } else if (name.compare("IntersectLineLine") == 0) {
    result = new IntersectLineLine();
  } else if (name.compare("IntersectLinePlane") == 0) {
    result = new IntersectLinePlane();
  } else if (name.compare("IntersectPlanePlane") == 0) {
    result = new IntersectPlanePlane();
  } else if (name.compare("ChangeRadius") == 0) {
    result = new ChangeRadius();
  } else if (name.compare("TranslateByValue") == 0) {
    result = new TranslateByValue();
  } else if (name.compare("Register") == 0) {
    result = new Register();
  } else if (name.compare("HelmertTransformation") == 0) {
    result = new Helmert7Param();
  } else if (name.compare("Helmert6Param") == 0) {
    result = new Helmert6Param();
  } else if (name.compare("BestFitCircle") == 0) {
    result = new BestFitCircle();
  } else if (name.compare("BestFitCircleInPlane") == 0) {
    result = new BestFitCircleInPlane();
  } else if (name.compare("BestFitSphere") == 0) {
    result = new BestFitSphere();
  } else if (name.compare("RectifyToPoint") == 0) {
    result = new RectifyToPoint();
  } else if (name.compare("Shift") == 0) {
    result = new Shift();
  } else if (name.compare("RectifyToVector") == 0) {
    return new RectifyToVector();
  } else if (name.compare("BestFitCylinder") == 0) {
    return new BestFitCylinder();
  } else if (name.compare("BestFitCylinderAppxDirection") == 0) {
    return new BestFitCylinderAppxDirection();
  } else if (name.compare("BestFitCylinderAppxDummyPoint") == 0) {
    return new BestFitCylinderAppxDummyPoint();
  } else {
    return createFunctionFromConfig(name);
  }
  return result;
}

oi::Guarded<Function> OiTemplatePlugin::createFunctionFromConfig(
    const QString& functionName) {
  // internal functions
  if (functionName.compare("DistanceBetweenTwoPoints") == 0) {
    return new DistanceBetweenTwoPoints();
  } else if (functionName.compare("XYFilter") == 0) {
    return new XYZFilter("XYFilter", true, true, false);  // keep Z
  } else if (functionName.compare("XZFilter") == 0) {
    return new XYZFilter("XZFilter", true, false, true);  // keep Y
  } else if (functionName.compare("YZFilter") == 0) {
    return new XYZFilter("YZFilter", false, true, true);  // keep X
  }

  oi::Guarded<Function> function(nullptr);

  FunctionConfigParser parser;
  foreach (ConfiguredFunctionConfig config, parser.readConfigFromJson()) {
    if (config.name.compare(functionName) == 0) {
      QList<oi::Guarded<Function> > functions;
      foreach (QString name, config.getFunctionNames()) {
        oi::Guarded<Function> f = createFunction(name);
        if (f.isNull()) {
          throw logic_error(
              QString("no function found: %1").arg(name).toLocal8Bit().data());
        }
        f->init();
        functions.append(f);
      }

      if (config.version == 2) {
        function = new ConfiguredFunction(config, functions);
        function->init();
        break;
      }
    }

    if (!function.isNull()) {
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
oi::Guarded<BundleAdjustment> OiTemplatePlugin::createBundleAdjustment(
    const QString& name) {
  oi::Guarded<BundleAdjustment> result(nullptr);
  return result;
}

/*!
 * \brief OiTemplatePlugin::createSimulation
 * Returns the simulation with the specified name
 * \param name
 * \return
 */
oi::Guarded<SimulationModel> OiTemplatePlugin::createSimulation(
    const QString& name) {
  oi::Guarded<SimulationModel> result(nullptr);

  if (name.compare("SimplePolarMeasurement") == 0) {
    result = new SimplePolarMeasurement();
  }

  return result;
}

/*!
 * \brief OiTemplatePlugin::createSimpleAsciiExchange
 * \param name
 * \return
 */
oi::Guarded<ExchangeSimpleAscii> OiTemplatePlugin::createSimpleAsciiExchange(
    const QString& name) {
  oi::Guarded<ExchangeSimpleAscii> result(nullptr);

  if (name.compare("OiExchangeAscii") == 0) {
    result = new OiExchangeAscii();
  }

  return result;
}

/*!
 * \brief OiTemplatePlugin::createDefinedFormatExchange
 * \param name
 * \return
 */
oi::Guarded<ExchangeDefinedFormat>
OiTemplatePlugin::createDefinedFormatExchange(const QString& name) {
  oi::Guarded<ExchangeDefinedFormat> result(nullptr);
  return result;
}
