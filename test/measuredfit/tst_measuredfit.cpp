#include <QtTest>
#include <cmath>
#include <vector>

#include "chooselalib.h"
#include "containers.h"
#include "featurewrapper.h"
#include "geometry/cylinder.h"
#include "geometry/plane.h"
#include "geometry/point.h"
#include "oijob.h"
#include "p_bestfitcylinder.h"
#include "p_bestfitplane.h"
#include "reading.h"
#include "station.h"

using namespace oi;
using namespace oi::math;

namespace {

oi::Guarded<Reading> makeCartesianReading(double x, double y, double z) {
  ReadingCartesian r;
  r.isValid = true;
  r.xyz.setAt(0, x);
  r.xyz.setAt(1, y);
  r.xyz.setAt(2, z);
  return new Reading(r);
}

}  // namespace

/*!
 * Stage 7c-ii's own acceptance criterion, quoted verbatim from the plan:
 * "measuring into a plane must produce a solved plane end to end - tagged
 * points from acquisition, resolved by tag, fitted." Everything up to this
 * stage (7f) proved tag resolution feeds a *stand-in* consumer
 * (tst_measurementintent.cpp's FakeConsumer, in openindy-core, which has
 * no plugin to link against) with no binding step. What that could not
 * prove is that a *real* fit function - one still declaring
 * eObservationElement before this stage - actually receives those points
 * and solves. This is the missing link: the real BestFitPlane/
 * BestFitCylinder classes, driven purely through OiJob::addMeasurementResults
 * and OiJob::resolvePointIntent, with no direct addInputElement call
 * anywhere in this file.
 *
 * Like tst_measurementintent.cpp, this test stands in for the
 * TrafoController/FeatureUpdater transform step core does not have
 * (oijob.cpp's own comment on addMeasurementResults explains why: an
 * Observation's xyz is not meaningful until the app-level transform runs).
 * measureOnePoint() does that stand-in, then everything else - the tag
 * stamp, the resolution into the function's InputElements, the fit itself -
 * is the real production code path.
 */
class MeasuredFitTest : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();

  void measuringIntoAPlaneProducesASolvedPlaneEndToEnd();
  void measuringIntoACylinderProducesASolvedCylinderEndToEnd();

 private:
  oi::Guarded<OiJob> makeJobWithActiveStation(oi::Guarded<Station>& outStation);
  oi::Guarded<Point> measureOnePoint(const oi::Guarded<OiJob>& job,
                                     int targetId, double x, double y,
                                     double z);
};

void MeasuredFitTest::initTestCase() {
  ChooseLALib::setLinearAlgebra(ChooseLALib::LinearAlgebraLibrary::Armadillo);
}

oi::Guarded<OiJob> MeasuredFitTest::makeJobWithActiveStation(
    oi::Guarded<Station>& outStation) {
  oi::Guarded<OiJob> job = new OiJob();

  auto own_station = std::make_unique<oi::Station>("S1");
  oi::Guarded<oi::Station> station = own_station.get();
  oi::Guarded<oi::FeatureWrapper> stationFeature = station->getFeatureWrapper();
  job->addFeature(std::move(own_station));

  station->setActiveStationState(true);

  outStation = station;
  return job;
}

/*!
 * \brief One full measurement: addMeasurementResults (the real acquisition
 * entry point) creates and tags a new Point, resolvePointIntent feeds any
 * matching function - then this stands in for the app-level transform
 * (FeatureUpdater/TrafoController, which core does not have) so the point
 * actually solves, exactly as tst_measurementintent.cpp's own tests do.
 */
oi::Guarded<Point> MeasuredFitTest::measureOnePoint(
    const oi::Guarded<OiJob>& job, int targetId, double x, double y, double z) {
  const std::vector<oi::Guarded<FeatureWrapper> > before =
      job->getFeaturesList();

  std::vector<oi::Guarded<Reading> > readings;
  readings.push_back(makeCartesianReading(x, y, z));
  job->addMeasurementResults(targetId, readings);

  oi::Guarded<Point> measured;
  for (const oi::Guarded<FeatureWrapper>& fw : job->getFeaturesList()) {
    if (fw.isNull() || fw->getPoint().isNull()) {
      continue;
    }
    if (!oi::contains(before, fw)) {
      measured = fw->getPoint();
    }
  }

  if (measured.isNull()) {
    return measured;
  }

  for (const oi::Guarded<Observation>& obs : measured->getObservations()) {
    OiVec xyz(4);
    xyz.setAt(0, x);
    xyz.setAt(1, y);
    xyz.setAt(2, z);
    xyz.setAt(3, 1.0);
    obs->setXYZ(xyz);
    obs->setIsSolved(true);
  }
  measured->recalc();

  return measured;
}

void MeasuredFitTest::measuringIntoAPlaneProducesASolvedPlaneEndToEnd() {
  oi::Guarded<Station> station;
  oi::Guarded<OiJob> job = makeJobWithActiveStation(station);

  auto own_plane = std::make_unique<oi::Plane>();
  oi::Guarded<oi::Plane> plane = own_plane.get();
  oi::Guarded<oi::FeatureWrapper> planeFeature = plane->getFeatureWrapper();
  planeFeature->getFeature()->setFeatureName("wall-01");
  job->addFeature(std::move(own_plane));

  oi::Guarded<Function> function = new BestFitPlane();
  function->init();
  plane->addFunction(function);

  QVERIFY2(oi::value(function->getInputElements(), 0).empty(),
           "nothing tagged yet");

  const int targetId = planeFeature->getFeature()->getId();

  // four points on z = 0.5x + 0.25y - not axis-aligned, so a fit could
  // not satisfy this by accident
  struct Pt {
    double x, y, z;
  };
  const std::vector<Pt> pts = {
      {0.0, 0.0, 0.0}, {2.0, 0.0, 1.0}, {0.0, 2.0, 0.5}, {2.0, 2.0, 1.5}};
  for (const Pt& p : pts) {
    QVERIFY2(!measureOnePoint(job, targetId, p.x, p.y, p.z).isNull(),
             "measuring must create a new point each time");
  }

  // the acceptance criterion itself: four points arrived in the function's
  // InputElements with no addInputElement call anywhere in this test -
  // tag resolution alone did it
  QCOMPARE(oi::value(function->getInputElements(), 0).size(), 4);

  // recalc(), not exec() directly - this is what makes it end to end:
  // the same path the app takes, which is also what actually flips
  // Plane::isSolved
  plane->recalc();
  QVERIFY2(plane->getIsSolved(),
           "plane must be solved after measuring four points into it");

  // true normal for z = 0.5x + 0.25y, i.e. -0.5x - 0.25y + z = 0
  OiVec trueNormal(3);
  trueNormal.setAt(0, -0.5);
  trueNormal.setAt(1, -0.25);
  trueNormal.setAt(2, 1.0);
  const double trueNorm = std::sqrt(trueNormal.getAt(0) * trueNormal.getAt(0) +
                                    trueNormal.getAt(1) * trueNormal.getAt(1) +
                                    trueNormal.getAt(2) * trueNormal.getAt(2));

  const OiVec fittedNormal = plane->getDirection().getVector();
  const double fittedNorm =
      std::sqrt(fittedNormal.getAt(0) * fittedNormal.getAt(0) +
                fittedNormal.getAt(1) * fittedNormal.getAt(1) +
                fittedNormal.getAt(2) * fittedNormal.getAt(2));

  double dot = (trueNormal.getAt(0) * fittedNormal.getAt(0) +
                trueNormal.getAt(1) * fittedNormal.getAt(1) +
                trueNormal.getAt(2) * fittedNormal.getAt(2)) /
               (trueNorm * fittedNorm);
  dot = std::clamp(dot, -1.0, 1.0);
  const double angleErrorDeg = std::acos(std::abs(dot)) * 180.0 / M_PI;

  QVERIFY2(angleErrorDeg < 0.01,
           (std::format("fitted plane normal off by {} degrees", angleErrorDeg))
               .c_str());
}

void MeasuredFitTest::measuringIntoACylinderProducesASolvedCylinderEndToEnd() {
  oi::Guarded<Station> station;
  oi::Guarded<OiJob> job = makeJobWithActiveStation(station);

  auto own_cylinder = std::make_unique<oi::Cylinder>();
  oi::Guarded<oi::Cylinder> cylinder = own_cylinder.get();
  oi::Guarded<oi::FeatureWrapper> cylinderFeature =
      cylinder->getFeatureWrapper();
  cylinderFeature->getFeature()->setFeatureName("post-01");
  job->addFeature(std::move(own_cylinder));

  // D12/Stage 7c-ii: cylinder is not a special case - ePointElement with
  // a minimum of 5, exercised here with the same real-world dataset
  // testVRadial (tst_function.cpp) already proves converges to
  // radius ~19.16
  oi::Guarded<Function> function = new BestFitCylinder();
  function->init();
  ScalarInputParams scalarInputParams;
  scalarInputParams.stringParameter.insert_or_assign("approximation",
                                                     "guess axis");
  function->setScalarInputParams(scalarInputParams);
  cylinder->addFunction(function);

  QVERIFY2(oi::value(function->getInputElements(), 0).empty(),
           "nothing tagged yet");

  const int targetId = cylinderFeature->getFeature()->getId();

  struct Pt {
    double x, y, z;
  };
  const std::vector<Pt> pts = {
      {-3283.654, -79.927, 194.917}, {-3271.578, -84.991, 203.643},
      {-3292.599, -64.647, 196.517}, {-3308.806, -73.417, 213.805},
      {-3301.555, -87.819, 210.971}, {-3289.428, -79.973, 198.027},
      {-3292.505, -57.849, 200.595}, {-3303.342, -63.362, 213.794}};
  for (const Pt& p : pts) {
    QVERIFY2(!measureOnePoint(job, targetId, p.x, p.y, p.z).isNull(),
             "measuring must create a new point each time");
  }

  QCOMPARE(oi::value(function->getInputElements(), 0).size(), 8);

  cylinder->recalc();
  QVERIFY2(cylinder->getIsSolved(),
           "cylinder must be solved after measuring eight points into it");

  QVERIFY2(std::abs(cylinder->getRadius().getRadius() - 19.16) < 0.05,
           (std::format("fitted radius {}, expected ~19.16",
                        cylinder->getRadius().getRadius()))
               .c_str());
}

QTEST_APPLESS_MAIN(MeasuredFitTest)
#include <format>

#include "tst_measuredfit.moc"
