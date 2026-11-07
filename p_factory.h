#ifndef P_FACTORY_H
#define P_FACTORY_H
#include <memory>
#include <string>
#include <vector>

#include "bundleadjustment.h"
#include "configuredfunction.h"
#include "configuredfunctionconfig.h"
#include "constructfunction.h"
#include "distancebetweentwopoints.h"
#include "fitfunction.h"
#include "function.h"
#include "functionconfigparser.h"
#include "geodeticfunction.h"
#include "lasertracker.h"
#include "objecttransformation.h"
#include "p_averagefrompoints.h"
#include "p_bestfitcircle.h"
#include "p_bestfitcircleinplane.h"
#include "p_bestfitcylinder.h"
#include "p_bestfitline.h"
#include "p_bestfitplane.h"
#include "p_bestfitpoint.h"
#include "p_bestfitsphere.h"
#include "p_changeradius.h"
#include "p_helmert6param.h"
#include "p_helmert7Param.h"
#include "p_intersectlineline.h"
#include "p_intersectlineplane.h"
#include "p_intersectplaneplane.h"
#include "p_linefromplanenormal.h"
#include "p_nominalpoint.h"
#include "p_oiexchangeascii.h"
#include "p_planefromplane.h"
#include "p_pointbetweentwopoints.h"
#include "p_pointfromplaneorigin.h"
#include "p_pointfrompoints.h"
#include "p_pseudotracker.h"
#include "p_rectifytopoint.h"
#include "p_rectifytovector.h"
#include "p_register.h"
#include "p_shift.h"
#include "p_translatebyvalue.h"
#include "plugin.h"
#include "sensor.h"
#include "sensorconfiguration.h"
#include "simplepolarmeasurement.h"
#include "simulationmodel.h"
#include "systemtransformation.h"
#include "totalstation.h"
#include "xyzfilter.h"

using namespace oi;

class OiTemplatePlugin : public Plugin {
 public:
  OiTemplatePlugin();

  // ##########################################################
  // create instances of all available plugin types in a plugin
  // ##########################################################

  std::vector<std::unique_ptr<Sensor>> createSensors();
  std::vector<std::unique_ptr<Function>> createFunctions();
  std::vector<std::unique_ptr<BundleAdjustment>> createBundleAdjustments();
  std::vector<std::unique_ptr<SimulationModel>> createSimulations();
  std::vector<std::unique_ptr<ExchangeSimpleAscii>>
  createSimpleAsciiExchanges();
  std::vector<std::unique_ptr<ExchangeDefinedFormat>>
  createDefinedFormatExchanges();

  // ###################################################
  // create instances of a plugin type with a given name
  // ###################################################

  std::unique_ptr<Sensor> createSensor(const std::string& name);
  std::unique_ptr<Function> createFunction(const std::string& name);
  std::unique_ptr<Function> createFunctionFromConfig(const std::string& name);
  std::unique_ptr<BundleAdjustment> createBundleAdjustment(
      const std::string& name);
  std::unique_ptr<SimulationModel> createSimulation(const std::string& name);
  std::unique_ptr<ExchangeSimpleAscii> createSimpleAsciiExchange(
      const std::string& name);
  std::unique_ptr<ExchangeDefinedFormat> createDefinedFormatExchange(
      const std::string& name);
};

#endif  // P_FACTORY_H
