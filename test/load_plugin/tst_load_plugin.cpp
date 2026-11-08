#include <QPointer>
#include <QtTest>
#include <map>
#include <string>
#include <vector>

#include "choose_la_lib.h"
#include "plugin.h"
#include "plugin_loader.h"

using namespace oi;

class LoadPluginTest : public QObject {
  Q_OBJECT

 public:
  LoadPluginTest();

 private Q_SLOTS:
  void initTestCase();
  void testPseudoTracker_eCartesianReading();
  void testLoadPlugin();

 private:
};

LoadPluginTest::LoadPluginTest() {}

void LoadPluginTest::initTestCase() {
  // PseudoTracker's simulated readings go through OiMat rotation math
  // (TrackerErrorModel); without a backend selected here, that
  // dereferences a null linear-algebra pointer and crashes (main.cpp
  // does this same call at real app startup)
  ChooseLALib::setLinearAlgebra(ChooseLALib::LinearAlgebraLibrary::Armadillo);
}

/*
 * PATH:
 * OpenIndy-DefaultPlugin\lib\OpenIndy-Core\bin\debug\;
 * OpenIndy-DefaultPlugin\lib\OpenIndy-Core\lib\OpenIndy-Math\bin\debug\;
 * OpenIndy-DefaultPlugin\lib\OpenIndy-Core\lib\OpenIndy-Math\lib\armadillo-3.910.0\examples\lib_win64\
 *
 * Cannot load library ... Das angegebene Modul wurde nicht gefunden.
 * -> dll kann nicht geladen werden, weil z. B. benötigte dll nicht im Pfad ist
 *
 * Cannot load library ... Die angegebene Prozedur wurde nicht gefunden.
 * -> ???
 */
void LoadPluginTest::testLoadPlugin() {
  const QString pluginPath = PLUGIN_PATH;

  QVERIFY2(QFile::exists(pluginPath), pluginPath.toUtf8().constData());
  oi::PluginLoadResult loaded =
      oi::PluginLoader::load(pluginPath.toStdString());
  QVERIFY2(loaded, loaded.error.c_str());

  Plugin* oiPlugin = loaded.plugin.get();
  QVERIFY2(
      loaded.plugin.metadataJson().find("\"isOiPlugin\"") != std::string::npos,
      "plugin metadata missing");

  std::vector<std::unique_ptr<Function>> functions =
      oiPlugin->createFunctions();
  QVERIFY2(functions.size() > 0, "no functions found");
}
void LoadPluginTest::testPseudoTracker_eCartesianReading() {
  const QString pluginPath = PLUGIN_PATH;

  QVERIFY2(QFile::exists(pluginPath), pluginPath.toUtf8().constData());
  oi::PluginLoadResult loaded =
      oi::PluginLoader::load(pluginPath.toStdString());
  QVERIFY2(loaded, loaded.error.c_str());

  Plugin* oiPlugin = loaded.plugin.get();

  std::unique_ptr<Sensor> sensor = oiPlugin->createSensor("PseudoTracker");
  QVERIFY2(sensor != nullptr, "no sensor");

  sensor->init();

  MeasurementConfig config;
  config.setMeasurementType(MeasurementTypes::eSinglePoint_MeasurementType);
  SensorConfiguration sc = sensor->getSensorConfiguration();
  std::map<std::string, std::string> sp = sc.getStringParameter();
  sp.insert_or_assign("reading type", "cartesian");
  sc.setStringParameter(sp);
  sensor->setSensorConfiguration(sc);
  std::vector<std::unique_ptr<Reading>> readings = sensor->measure(config);
  QVERIFY2(readings.size() > 0, "no readings");
}

QTEST_APPLESS_MAIN(LoadPluginTest)

#include "tst_load_plugin.moc"
