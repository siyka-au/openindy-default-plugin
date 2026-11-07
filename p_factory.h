#ifndef P_FACTORY_H
#define P_FACTORY_H
#include <QObject>
#include <QString>
#include <QtGlobal>
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

class OiTemplatePlugin : public QObject, Plugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID Plugin_iidd FILE "metaInfo.json")
  Q_INTERFACES(Plugin)

 public:
  OiTemplatePlugin(QObject* parent = 0);

  // ##########################################################
  // create instances of all available plugin types in a plugin
  // ##########################################################

  std::vector<oi::Guarded<Sensor> > createSensors();
  std::vector<oi::Guarded<Function> > createFunctions();
  std::vector<oi::Guarded<BundleAdjustment> > createBundleAdjustments();
  std::vector<oi::Guarded<SimulationModel> > createSimulations();
  std::vector<oi::Guarded<ExchangeSimpleAscii> > createSimpleAsciiExchanges();
  std::vector<oi::Guarded<ExchangeDefinedFormat> >
  createDefinedFormatExchanges();

  // ###################################################
  // create instances of a plugin type with a given name
  // ###################################################

  oi::Guarded<Sensor> createSensor(const QString& name);
  oi::Guarded<Function> createFunction(const QString& name);
  oi::Guarded<Function> createFunctionFromConfig(const QString& name);
  oi::Guarded<BundleAdjustment> createBundleAdjustment(const QString& name);
  oi::Guarded<SimulationModel> createSimulation(const QString& name);
  oi::Guarded<ExchangeSimpleAscii> createSimpleAsciiExchange(
      const QString& name);
  oi::Guarded<ExchangeDefinedFormat> createDefinedFormatExchange(
      const QString& name);
};

#endif  // P_FACTORY_H
