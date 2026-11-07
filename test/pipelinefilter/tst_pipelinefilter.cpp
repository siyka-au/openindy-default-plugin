#include <QtTest>
#include <cmath>
#include <vector>

#include "chooselalib.h"
#include "containers.h"
#include "featurewrapper.h"
#include "geometry/plane.h"
#include "geometry/point.h"
#include "oijob.h"
#include "p_bestfitplane.h"
#include "p_bestfitpoint.h"
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
 * Stage 7b - filter pipelines on needed elements (D16, domain-model.md
 * S5), driven against the *real* BestFitPlane and BestFitPoint from this
 * plugin, exactly the way tst_measuredfit.cpp proves 7c-ii end to end:
 * OiJob::addMeasurementResults tags a new Point, resolvePointIntent feeds
 * it into the function's InputElements, and Function::exec() applies
 * whatever pipeline (openindy-core) was declared on that NeededElement
 * position - no direct addInputElement call, no plugin-side code changed
 * for either fit function.
 *
 * This is the literal acceptance criterion from the plan: "a plane fit
 * over a tagged point set containing deliberate outliers converges
 * correctly with the filter declared and incorrectly (or fails/degrades)
 * without it; the excluded points are individually reportable with a
 * reason; adding a new outlier re-triggers the filter and the fit." Plus
 * the explicit follow-up requirement to *wire*, not just assert by
 * symmetry, that the same mechanism works at observation level through
 * BestFitPoint (the one function touching raw Observations, D13).
 */
class PipelineFilterTest : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();

  void planeFitConvergesWithFilterDeclaredAndDegradesWithoutIt();
  void addingANewOutlierLaterReTriggersTheFilterAndTheFit();
  void observationLevelPipelineFiltersABadReadingInBestFitPoint();

 private:
  oi::Guarded<OiJob> makeJobWithActiveStation(oi::Guarded<Station>& outStation);
  oi::Guarded<Point> measureOnePoint(const oi::Guarded<OiJob>& job,
                                     int targetId, double x, double y,
                                     double z);
  double planeAngleErrorDeg(const oi::Guarded<Plane>& plane);
};

void PipelineFilterTest::initTestCase() {
  ChooseLALib::setLinearAlgebra(ChooseLALib::LinearAlgebraLibrary::Armadillo);
}

oi::Guarded<OiJob> PipelineFilterTest::makeJobWithActiveStation(
    oi::Guarded<Station>& outStation) {
  oi::Guarded<OiJob> job = new OiJob();

  oi::Guarded<Station> station = new Station("S1");
  oi::Guarded<FeatureWrapper> stationFeature = new FeatureWrapper();
  stationFeature->setStation(station);
  job->addFeature(stationFeature);

  station->setActiveStationState(true);

  outStation = station;
  return job;
}

// identical shape to tst_measuredfit.cpp's own helper: addMeasurementResults
// is the real acquisition entry point (tags + resolves), and this stands in
// for the app-level transform step (FeatureUpdater/TrafoController) core
// does not have, exactly as that test's own comment explains.
oi::Guarded<Point> PipelineFilterTest::measureOnePoint(
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

double PipelineFilterTest::planeAngleErrorDeg(const oi::Guarded<Plane>& plane) {
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
  return std::acos(std::abs(dot)) * 180.0 / M_PI;
}

void PipelineFilterTest::
    planeFitConvergesWithFilterDeclaredAndDegradesWithoutIt() {
  // 64 points exactly on z = 0.5x + 0.25y, plus two deliberate gross
  // outliers. A naive population-stdev sigma filter is only unmasked by
  // its own outliers once the sample is large enough: for k outliers of
  // residual R among N points, eVal (bestFitPlane's smallest PCA
  // eigenvalue, dominated by the outliers' own contribution) gives
  // stdev = std::sqrt(eVal/(N-3)) ~= R*sqrt(k/(N-3)), and a 3-sigma
  // threshold only exceeds R once N > 9k+3. With k=2 that is N > 21 -
  // this suite uses 64 good points for a comfortable margin (a real
  // inspection dataset is this size or larger, so this is
  // representative, not a test artefact). See
  // openindy-core/test/pipeline/tst_pipeline.cpp for the k=1 derivation
  // this generalises from.
  //
  // Non-collinear first three points, deliberately: BestFitPlane picks
  // its normal's *sign* (not magnitude - that comes from PCA over every
  // used point) from the cross product of usedPositions[0..2]
  //(p_bestfitplane.cpp), which degenerates for three collinear points -
  // a grid iterated x-outer/y-inner starts with several same-x points,
  // so build an L-shaped start explicitly and let the rest fill the grid.
  struct Pt {
    double x, y, z;
  };
  std::vector<Pt> pts;
  pts.push_back(Pt{0.0, 0.0, 0.0});
  pts.push_back(Pt{1.0, 0.0, 0.5});
  pts.push_back(Pt{0.0, 1.0, 0.25});
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      if ((x == 0 && y == 0) || (x == 1 && y == 0) || (x == 0 && y == 1)) {
        continue;  // already added above
      }
      pts.push_back(Pt{double(x), double(y), 0.5 * x + 0.25 * y});
    }
  }
  // two deliberate outliers - large relative to the good points'
  // residual noise (which is exactly zero here, a perfect plane), but
  // not so extreme that they hijack the PCA fit's own axis selection (a
  // huge outlier against a small-spread plane can rotate which
  // eigenvector *is* the smallest, which is a different failure mode
  // than "a 3-sigma filter fails to flag it" - see domain-model.md S9
  // for why bestFitPlane's degeneracy checks are a hint, not a gate)
  pts.push_back(Pt{1.0, 1.0, 8.75});   // true z=0.75, deliberate outlier 1
  pts.push_back(Pt{2.0, 3.0, -4.25});  // true z=1.75, deliberate outlier 2

  //--- without the filter: the outliers must degrade the fit ---
  {
    oi::Guarded<Station> station;
    oi::Guarded<OiJob> job = this->makeJobWithActiveStation(station);

    oi::Guarded<Plane> plane = new Plane();
    oi::Guarded<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);
    planeFeature->getFeature()->setFeatureName("wall-nofilter");
    job->addFeature(planeFeature);

    oi::Guarded<Function> function = new BestFitPlane();
    function->init();
    plane->addFunction(function);

    const int targetId = planeFeature->getFeature()->getId();
    for (const Pt& p : pts) {
      QVERIFY(!this->measureOnePoint(job, targetId, p.x, p.y, p.z).isNull());
    }
    QCOMPARE(oi::value(function->getInputElements(), 0).size(), pts.size());

    plane->recalc();
    QVERIFY2(plane->getIsSolved(),
             "plane must still solve (a fit tolerates outliers, it just gets "
             "the wrong answer)");

    const double angleErrorDeg = this->planeAngleErrorDeg(plane);
    QVERIFY2(angleErrorDeg > 1.0,
             (std::format("expected the undeclared-filter fit to be "
                          "measurably off, got {} degrees",
                          angleErrorDeg))
                 .c_str());
  }

  //--- with the filter declared: the fit converges correctly, and the
  // outliers are individually reportable with a reason ---
  {
    oi::Guarded<Station> station;
    oi::Guarded<OiJob> job = this->makeJobWithActiveStation(station);

    oi::Guarded<Plane> plane = new Plane();
    oi::Guarded<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);
    planeFeature->getFeature()->setFeatureName("wall-filtered");
    job->addFeature(planeFeature);

    oi::Guarded<Function> function = new BestFitPlane();
    function->init();
    function->setPipeline(0, std::vector<PipelineStage>{
                                 PipelineStage::residualOutlierRejection(3.0)});
    plane->addFunction(function);

    const int targetId = planeFeature->getFeature()->getId();
    std::vector<oi::Guarded<Point> > measured;
    for (const Pt& p : pts) {
      oi::Guarded<Point> m =
          this->measureOnePoint(job, targetId, p.x, p.y, p.z);
      QVERIFY(!m.isNull());
      measured.push_back(m);
    }
    QCOMPARE(oi::value(function->getInputElements(), 0).size(), pts.size());

    plane->recalc();
    QVERIFY2(plane->getIsSolved(),
             "plane must be solved after measuring into it");

    const double angleErrorDeg = this->planeAngleErrorDeg(plane);
    QVERIFY2(
        angleErrorDeg < 0.1,
        (std::format("expected the filtered fit to converge, got {} degrees",
                     angleErrorDeg))
            .c_str());

    // the two deliberate outliers are the last two measured points -
    // both must be excluded, and both must be individually reportable
    // with a reason (traceability requirement)
    const std::vector<PipelineExclusion> exclusions =
        function->getPipelineExclusions(0);
    QVERIFY2(exclusions.size() >= 2,
             (std::format("expected at least 2 exclusions, got {}",
                          exclusions.size()))
                 .c_str());

    const int outlier1Id = measured.at(measured.size() - 2)->getId();
    const int outlier2Id = measured.at(measured.size() - 1)->getId();
    bool found1 = false, found2 = false;
    for (const PipelineExclusion& excl : exclusions) {
      if (excl.elementId == outlier1Id) {
        found1 = true;
        QVERIFY(!excl.reason.empty());
      }
      if (excl.elementId == outlier2Id) {
        found2 = true;
        QVERIFY(!excl.reason.empty());
      }
    }
    QVERIFY2(found1 && found2,
             "both deliberate outliers must be individually reportable with a "
             "reason");

    // and every genuinely good point must still be in use
    for (const Pt& p : pts) {
      Q_UNUSED(p);
    }
    for (int i = 0; i < measured.size() - 2; i++) {
      QVERIFY2(function->getShouldBeUsed(0, measured.at(i)->getId()),
               "a genuinely good point must not be caught by the filter");
    }
  }
}

void PipelineFilterTest::addingANewOutlierLaterReTriggersTheFilterAndTheFit() {
  oi::Guarded<Station> station;
  oi::Guarded<OiJob> job = this->makeJobWithActiveStation(station);

  oi::Guarded<Plane> plane = new Plane();
  oi::Guarded<FeatureWrapper> planeFeature = new FeatureWrapper();
  planeFeature->setPlane(plane);
  planeFeature->getFeature()->setFeatureName("wall-retrigger");
  job->addFeature(planeFeature);

  oi::Guarded<Function> function = new BestFitPlane();
  function->init();
  function->setPipeline(0, std::vector<PipelineStage>{
                               PipelineStage::residualOutlierRejection(3.0)});
  plane->addFunction(function);

  const int targetId = planeFeature->getFeature()->getId();

  // non-collinear first three points, and a large-enough sample for a
  // single new outlier (k=1 => N > 12, see the comment in
  // planeFitConvergesWithFilterDeclaredAndDegradesWithoutIt() above) -
  // 36 good points here for margin
  struct Pt {
    double x, y, z;
  };
  std::vector<Pt> pts;
  pts.push_back(Pt{0.0, 0.0, 0.0});
  pts.push_back(Pt{1.0, 0.0, 0.5});
  pts.push_back(Pt{0.0, 1.0, 0.25});
  for (int x = 0; x < 6; x++) {
    for (int y = 0; y < 6; y++) {
      if ((x == 0 && y == 0) || (x == 1 && y == 0) || (x == 0 && y == 1)) {
        continue;
      }
      pts.push_back(Pt{double(x), double(y), 0.5 * x + 0.25 * y});
    }
  }
  for (const Pt& p : pts) {
    QVERIFY(!this->measureOnePoint(job, targetId, p.x, p.y, p.z).isNull());
  }

  plane->recalc();
  QVERIFY(function->getPipelineExclusions(0).empty());
  QVERIFY2(this->planeAngleErrorDeg(plane) < 0.1,
           "clean data must fit correctly before any outlier arrives");

  // a new gross-but-not-extreme outlier is measured later (see the note
  // in the sibling test above re: outlier magnitude vs PCA axis
  // hijacking) - no pipeline redeclaration
  oi::Guarded<Point> outlier =
      this->measureOnePoint(job, targetId, 1.0, 1.0, 9.0);
  QVERIFY(!outlier.isNull());

  plane->recalc();

  QVERIFY2(!function->getShouldBeUsed(0, outlier->getId()),
           "the newly-added outlier must be caught without redeclaring the "
           "pipeline");
  QVERIFY2(
      this->planeAngleErrorDeg(plane) < 0.1,
      "the fit must still converge correctly once the new outlier is filtered");
}

/*!
 * The same mechanism at observation level (BestFitPoint, the one function
 * touching raw Observations per D13) - verified by actually wiring a
 * pipeline onto a real BestFitPoint instance, not asserted by symmetry
 * with the Point-level case above.
 */
void PipelineFilterTest::
    observationLevelPipelineFiltersABadReadingInBestFitPoint() {
  oi::Guarded<Station> station;
  oi::Guarded<OiJob> job = this->makeJobWithActiveStation(station);

  oi::Guarded<Point> point = new Point();
  oi::Guarded<FeatureWrapper> pointFeature = new FeatureWrapper();
  pointFeature->setPoint(point);
  pointFeature->getFeature()->setFeatureName("target-01");
  job->addFeature(pointFeature);

  oi::Guarded<Function> function = new BestFitPoint();
  function->init();
  function->setPipeline(0, std::vector<PipelineStage>{
                               PipelineStage::residualOutlierRejection(3.0)});
  point->addFunction(function);

  // 20 good readings clustered tightly around (10, 20, 30), plus one
  // grossly mis-aimed reading. eObservationElement, position 0 - the
  // same NeededElement::pipeline mechanism, no plugin-side code touched
  // to make BestFitPoint honour it.
  std::vector<oi::Guarded<Observation> > observations;
  auto addReading = [&](double x, double y, double z) {
    oi::Guarded<Reading> reading = makeCartesianReading(x, y, z);
    oi::Guarded<Observation> observation = new Observation();
    reading->setObservation(observation);
    station->getCoordinateSystem()->addObservation(observation);
    OiVec xyz(4);
    xyz.setAt(0, x);
    xyz.setAt(1, y);
    xyz.setAt(2, z);
    xyz.setAt(3, 1.0);
    observation->setXYZ(xyz);
    observation->setIsSolved(true);
    point->addObservation(observation);
    observations.push_back(observation);
    InputElement element(observation->getId());
    element.typeOfElement = ElementTypes::eObservationElement;
    element.observation = observation;
    function->addInputElement(element, 0);
  };

  for (int i = 0; i < 20; i++) {
    addReading(10.0, 20.0, 30.0);
  }
  addReading(10.0, 20.0, 930.0);  // the deliberate bad reading

  point->recalc();

  QVERIFY2(point->getIsSolved(),
           "point must be solved after fitting its observations");
  QVERIFY2(
      std::abs(point->getPosition().getVector().getAt(2) - 30.0) < 1.0,
      (std::format("expected z near 30 with the bad reading filtered, got {}",
                   point->getPosition().getVector().getAt(2)))
          .c_str());

  const int badId = observations.back()->getId();
  QVERIFY2(!function->getShouldBeUsed(0, badId),
           "the mis-aimed reading must be excluded");

  const std::vector<PipelineExclusion> exclusions =
      function->getPipelineExclusions(0);
  bool found = false;
  for (const PipelineExclusion& excl : exclusions) {
    if (excl.elementId == badId) {
      found = true;
      QVERIFY(!excl.reason.empty());
    }
  }
  QVERIFY2(
      found,
      "the excluded observation must be individually reportable with a reason");
}

QTEST_APPLESS_MAIN(PipelineFilterTest)
#include <format>

#include "tst_pipelinefilter.moc"
