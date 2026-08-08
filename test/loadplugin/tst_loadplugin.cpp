#include <QString>
#include <QtTest>
#include <QPointer>
#include <QList>
#include "plugin.h"
#include "chooselalib.h"

using namespace oi;

class LoadPluginTest : public QObject
{
    Q_OBJECT

public:
    LoadPluginTest();

private Q_SLOTS:
    void initTestCase();
    void testPseudoTracker_eCartesianReading();
    void testLoadPlugin();

private:

};

LoadPluginTest::LoadPluginTest()
{
}

void LoadPluginTest::initTestCase(){
    // PseudoTracker's simulated readings go through OiMat rotation math
    // (TrackerErrorModel); without a backend selected here, that
    // dereferences a null linear-algebra pointer and crashes (main.cpp
    // does this same call at real app startup)
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
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
void LoadPluginTest::testLoadPlugin(){

    QString pluginPath = PLUGIN_PATH;

    QVERIFY2(QFile::exists(pluginPath), pluginPath.toLatin1().data());
    QPluginLoader pluginLoader(pluginPath);

    QObject *plugin = pluginLoader.instance();
    QVERIFY2(plugin, pluginLoader.errorString().toLatin1().data());

    Plugin *oiPlugin = qobject_cast<Plugin *>(plugin);
    QVERIFY2(oiPlugin, "qobject_cast");

    QList<QPointer<Function> > functions = oiPlugin->createFunctions();
    QVERIFY2(functions.size() > 0, "no functions found");

}
void LoadPluginTest::testPseudoTracker_eCartesianReading(){

    QString pluginPath = PLUGIN_PATH;

    QVERIFY2(QFile::exists(pluginPath), pluginPath.toLatin1().data());
    QPluginLoader pluginLoader(pluginPath);

    QObject *plugin = pluginLoader.instance();
    QVERIFY2(plugin, pluginLoader.errorString().toLatin1().data());

    Plugin *oiPlugin = qobject_cast<Plugin *>(plugin);
    QVERIFY2(oiPlugin, "qobject_cast");

    QPointer<Sensor> sensor = oiPlugin->createSensor("PseudoTracker");

    sensor->init();

    MeasurementConfig config;
    config.setMeasurementType(eSinglePoint_MeasurementType);
    SensorConfiguration sc = sensor->getSensorConfiguration();
    QMap<QString, QString> sp = sc.getStringParameter();
    sp.insert("reading type", "cartesian");
    sc.setStringParameter(sp);
    sensor->setSensorConfiguration(sc);
    QList<QPointer<Reading> > readings = sensor->measure(config);
    QVERIFY2(readings.size() > 0, "no readings");

}

QTEST_APPLESS_MAIN(LoadPluginTest)

#include "tst_loadplugin.moc"
