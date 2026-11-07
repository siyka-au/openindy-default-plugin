#include <QPointer>
#include <QtTest>
#include <sstream>
#include <string>
#include <vector>

#include "chooselalib.h"
#include "featurewrapper.h"
#include "p_oiexchangeascii.h"
#include "types.h"

using namespace oi;

class OiExchangeAsciiTest : public QObject {
  Q_OBJECT

 public:
  OiExchangeAsciiTest();

 private Q_SLOTS:
  void testImportNominal();
  void testImportLevel();
  void testImportReportsProgressAndCanBeCancelled();
};

OiExchangeAsciiTest::OiExchangeAsciiTest() {}

void OiExchangeAsciiTest::testImportNominal() {
  std::string data(
      "\
Name	x	y	z\r\n\
P1004	-969.17	-566.68	-514.68\r\n\
P14	-956.26	-224.04	302.29\r\n\
P16	-876.67	1052.96	281.97\r\n\
P17	-870.8	1034.19	-450.19\r\n\
P19	-802.72	2226.45	296.64\r\n\
P20	-749.56	2987.57	-453.59\r\n\
");

  // DataExchanger::importData line 115 ...
  OiExchangeAscii* exchange = new OiExchangeAscii();
  exchange->init();

  // setup
  exchange->setGeometryType(GeometryTypes::ePointGeometry);
  exchange->setSkipFirstLine(true);
  exchange->setDelimiter("whitespace [ ]");

  std::vector<ExchangeSimpleAscii::ColumnType> columns;
  columns.push_back(ExchangeSimpleAscii::eColumnFeatureName);
  columns.push_back(ExchangeSimpleAscii::eColumnX);
  columns.push_back(ExchangeSimpleAscii::eColumnY);
  columns.push_back(ExchangeSimpleAscii::eColumnZ);
  exchange->setUserDefinedColumns(columns);

  std::stringstream stream(data);
  exchange->setStream(&stream);

  exchange->setFeatures(
      std::vector<oi::Guarded<FeatureWrapper> >());  // only export
  exchange->setExportObservations(false);            // only export
  exchange->setGroupName("Group01");
  exchange->setNominalSystem(oi::Guarded<CoordinateSystem>(new CoordinateSystem(
      oi::Guarded<Station>())) /*features.at(0)->getCoordinateSystem()*/);

  exchange->setUnit(DimensionType::eMetric, UnitType::eUnitMilliMeter);
  exchange->setUnit(DimensionType::eAngular, UnitType::eUnitDecimalDegree);
  exchange->setUnit(DimensionType::eTemperature, UnitType::eUnitGrad);

  // import data
  exchange->importOiData({});

  // test
  QVERIFY(exchange->getFeatures().size() == 6);

  oi::Guarded<Point> point = exchange->getFeatures().at(0)->getPoint();
  QCOMPARE(point->getFeatureName(), std::string("P1004"));
  QCOMPARE(point->getGroupName(), std::string("Group01"));
  OiVec xyz = point->getPosition().getVector();
  QCOMPARE(xyz.getAt(0), -.96917);
  QCOMPARE(xyz.getAt(1), -.56668);
  QCOMPARE(xyz.getAt(2), -.51468);

  QCOMPARE(exchange->getFeatures().at(1)->getPoint()->getFeatureName(),
           std::string("P14"));
  QCOMPARE(exchange->getFeatures().at(2)->getPoint()->getFeatureName(),
           std::string("P16"));
  QCOMPARE(exchange->getFeatures().at(3)->getPoint()->getFeatureName(),
           std::string("P17"));
  QCOMPARE(exchange->getFeatures().at(4)->getPoint()->getFeatureName(),
           std::string("P19"));
  QCOMPARE(exchange->getFeatures().at(5)->getPoint()->getFeatureName(),
           std::string("P20"));

  delete exchange;
}

namespace {
struct RecordingProgress : oi::ProgressSink {
  std::vector<oi::Progress> reports;
  void onProgress(const oi::Progress& p) override { reports.push_back(p); }
};
struct FinishObserver : oi::ExchangeObserver {
  int finished = -1;
  void onImportFinished(bool success) override { finished = success ? 1 : 0; }
};
}  // namespace

void OiExchangeAsciiTest::testImportReportsProgressAndCanBeCancelled() {
  ChooseLALib::setLinearAlgebra(ChooseLALib::LinearAlgebraLibrary::Armadillo);
  const std::string data = "P1 1 2 3\nP2 4 5 6\nP3 7 8 9\nP4 1 1 1\n";

  auto run = [&](const oi::OperationContext& ctx, FinishObserver& observer) {
    OiExchangeAscii exchange;
    exchange.init();
    exchange.setGeometryType(GeometryTypes::ePointGeometry);
    exchange.setSkipFirstLine(false);
    exchange.setDelimiter("whitespace [ ]");
    exchange.setUserDefinedColumns(
        {ExchangeSimpleAscii::eColumnFeatureName, ExchangeSimpleAscii::eColumnX,
         ExchangeSimpleAscii::eColumnY, ExchangeSimpleAscii::eColumnZ});
    std::stringstream stream(data);
    exchange.setStream(&stream);
    exchange.setNominalSystem(oi::Guarded<CoordinateSystem>(
        new CoordinateSystem(oi::Guarded<Station>())));
    exchange.setUnit(DimensionType::eMetric, UnitType::eUnitMilliMeter);
    exchange.setUnit(DimensionType::eAngular, UnitType::eUnitDecimalDegree);
    exchange.setUnit(DimensionType::eTemperature, UnitType::eUnitGrad);
    exchange.setObserver(&observer);
    exchange.importOiData(ctx);
    return exchange.getFeatures().size();
  };

  RecordingProgress sink;
  FinishObserver done;
  QCOMPARE(run(oi::OperationContext(&sink), done), std::size_t{4});
  QCOMPARE(done.finished, 1);
  QCOMPARE(sink.reports.size(), std::size_t{4});
  double last = 0.0;
  for (const oi::Progress& p : sink.reports) {
    QVERIFY(p.isDeterminate());
    QVERIFY(p.fraction >= last);
    QVERIFY(p.fraction < 1.0);  // 100% is the host's to show on completion
    last = p.fraction;
  }
  QCOMPARE(sink.reports.back().message, std::string("4 nominal(s) loaded"));

  std::stop_source stop;
  stop.request_stop();
  FinishObserver cancelled;
  RecordingProgress quiet;
  QCOMPARE(run(oi::OperationContext(&quiet, stop.get_token()), cancelled),
           std::size_t{0});
  QCOMPARE(cancelled.finished, 0);
  QVERIFY(quiet.reports.empty());
}

void OiExchangeAsciiTest::testImportLevel() {
  ChooseLALib::setLinearAlgebra(ChooseLALib::LinearAlgebraLibrary::Armadillo);

  std::string data(
      "\
name x y z i j k\r\n\
# Level as Plane\r\n\
\r\n\
Level_1 0.0 0.0 0.0 0.001310 -0.001903 0.999997\r\n\
\r\n\
Level_2  0.0 0.0 0.0 0.001023 -0.001945 0.999989\r\n\
#Level_2  0.0 0.0 0.0 0.001018 -0.001951 0.999994\r\n\
");

  // DataExchanger::importData line 115 ...
  OiExchangeAscii* exchange = new OiExchangeAscii();
  exchange->init();

  // setup
  exchange->setGeometryType(GeometryTypes::ePlaneGeometry);
  exchange->setSkipFirstLine(true);
  exchange->setDelimiter("whitespace [ ]");

  std::stringstream stream(data);
  exchange->setStream(&stream);

  // read column count from file / device
  exchange->setUserDefinedColumns(
      exchange->getDefaultColumnOrder(GeometryTypes::ePlaneGeometry));

  exchange->setFeatures(
      std::vector<oi::Guarded<FeatureWrapper> >());  // only export
  exchange->setExportObservations(false);            // only export
  exchange->setGroupName("Group01");
  exchange->setNominalSystem(oi::Guarded<CoordinateSystem>(new CoordinateSystem(
      oi::Guarded<Station>())) /*features.at(0)->getCoordinateSystem()*/);

  exchange->setUnit(DimensionType::eMetric, UnitType::eUnitMilliMeter);
  exchange->setUnit(DimensionType::eAngular, UnitType::eUnitDecimalDegree);
  exchange->setUnit(DimensionType::eTemperature, UnitType::eUnitGrad);

  // import data
  exchange->importOiData({});

  // test
  QVERIFY(exchange->getFeatures().size() == 2);

  oi::Guarded<Plane> plane = exchange->getFeatures().at(0)->getPlane();
  QCOMPARE(plane->getFeatureName(), std::string("Level_1"));
  QCOMPARE(plane->getGroupName(), std::string("Group01"));
  OiVec xyz = plane->getPosition().getVector();
  QCOMPARE(xyz.getAt(0), .0);
  QCOMPARE(xyz.getAt(1), .0);
  QCOMPARE(xyz.getAt(2), .0);

  OiVec ijk = plane->getDirection().getVector();
  QCOMPARE(ijk.getAt(0), 0.001310);
  QCOMPARE(ijk.getAt(1), -0.001903);
  QCOMPARE(ijk.getAt(2), 0.999997);

  QCOMPARE(exchange->getFeatures().at(1)->getPlane()->getFeatureName(),
           std::string("Level_2"));

  delete exchange;
}

QTEST_APPLESS_MAIN(OiExchangeAsciiTest)

#include "tst_oiexchangeascii.moc"
