#include <QtTest>
#include <cmath>

#include "chooselalib.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "geometry/line.h"
#include "geometry/plane.h"
#include "geometry/point.h"
#include "oijob.h"
#include "p_linefromplanenormal.h"
#include "p_pointfromplaneorigin.h"

using namespace oi;
using namespace oi::math;

namespace {

Position pos(double x, double y, double z) {
  Position p;
  p.setVector(x, y, z);
  return p;
}

Direction dir(double i, double j, double k) {
  Direction d;
  OiVec v(3);
  v.setAt(0, i);
  v.setAt(1, j);
  v.setAt(2, k);
  d.setVector(v);
  return d;
}

/*!
 * \brief A minimal, real producer so the plane can become "solved" (and
 * re-solved to a new value) without a real BestFitPlane/points setup -
 * mutable so a test can simulate re-measuring the plane between recalcs.
 */
class SetPlaneFunction : public ConstructFunction, public Producer<Plane> {
  Q_OBJECT
 public:
  SetPlaneFunction(QObject* parent = 0) : ConstructFunction(parent) {
    this->metaData.name = "SetPlaneFunction";
    this->applicableFor.append(ePlaneFeature);
  }
  Position position;
  Direction direction;
  std::expected<Plane, QString> exec() override {
    Plane plane;
    Position p = position;
    Direction d = direction;
    plane.setPlane(p, d);
    return plane;
  }
};

}  // namespace

/*!
 * \brief Stage 7h - Plane's owned accessor set: NormalLine and
 * PointOnPlane, generated via OiJob::addOwnedSubGeometry using the exact
 * same LineFromPlaneNormal/PointFromPlaneOrigin classes 7c-i built as an
 * *independent, editable* feature (tst_derivedfeatures.cpp). What differs
 * here is only that addOwnedSubGeometry records an owner - the math is
 * unchanged and untested again here on purpose; this file is about the
 * ownership/regeneration/deletion mechanism actually working end to end
 * with real plugin functions, not a plugin-agnostic stand-in (that is
 * openindy-core's tst_ownedsubgeometry).
 */
class OwnedPlaneAccessorsTest : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();

  void generatedNormalLineIsBoundReadOnlyAndComputesTheNormal();
  void generatedPointOnPlaneIsBoundReadOnlyAndComputesTheOrigin();
  void movingThePlaneRegeneratesItsOwnedAccessors();
  void editingAGeneratedAccessorIsRejected();
  void deletingThePlaneDeletesItsOwnedAccessors();

 private:
  QPointer<OiJob> makeJobWithPlane(QPointer<FeatureWrapper>& outPlaneWrapper,
                                   QPointer<SetPlaneFunction>& outPlaneFn,
                                   const Position& position,
                                   const Direction& normal);
};

void OwnedPlaneAccessorsTest::initTestCase() {
  ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

QPointer<OiJob> OwnedPlaneAccessorsTest::makeJobWithPlane(
    QPointer<FeatureWrapper>& outPlaneWrapper,
    QPointer<SetPlaneFunction>& outPlaneFn, const Position& position,
    const Direction& normal) {
  QPointer<OiJob> job = new OiJob();

  QPointer<Plane> plane = new Plane();
  QPointer<FeatureWrapper> planeWrapper = new FeatureWrapper();
  planeWrapper->setPlane(plane);
  plane->setFeatureName("Plane1");
  job->addFeature(planeWrapper);

  QPointer<SetPlaneFunction> planeFn = new SetPlaneFunction();
  planeFn->position = position;
  planeFn->direction = normal;
  plane->addFunction(planeFn);
  plane->recalc();

  outPlaneWrapper = planeWrapper;
  outPlaneFn = planeFn;
  return job;
}

void OwnedPlaneAccessorsTest::
    generatedNormalLineIsBoundReadOnlyAndComputesTheNormal() {
  QPointer<FeatureWrapper> planeWrapper;
  QPointer<SetPlaneFunction> planeFn;
  QPointer<OiJob> job = this->makeJobWithPlane(
      planeWrapper, planeFn, pos(1.0, 2.0, 3.0), dir(0.0, 0.0, 1.0));

  QPointer<Line> line = new Line();
  QPointer<FeatureWrapper> lineWrapper = new FeatureWrapper();
  lineWrapper->setLine(line);
  line->setFeatureName("Plane1.NormalLine");
  job->addFeature(lineWrapper);

  QPointer<LineFromPlaneNormal> function = new LineFromPlaneNormal();
  function->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, lineWrapper, function, 0));

  // simulate the DAG walk (no FeatureUpdater in a plugin-level test)
  line->recalc();

  QVERIFY(line->getOwnerDescription().has_value());
  QVERIFY(line->getOwnerDescription()->contains("Plane1"));
  QVERIFY(line->getOwnerDescription()->contains("LineFromPlaneNormal"));

  QCOMPARE(line->getPosition().getVector().getAt(0), 1.0);
  QCOMPARE(line->getPosition().getVector().getAt(2), 3.0);
  QCOMPARE(line->getDirection().getVector().getAt(2), 1.0);
}

void OwnedPlaneAccessorsTest::
    generatedPointOnPlaneIsBoundReadOnlyAndComputesTheOrigin() {
  QPointer<FeatureWrapper> planeWrapper;
  QPointer<SetPlaneFunction> planeFn;
  QPointer<OiJob> job = this->makeJobWithPlane(
      planeWrapper, planeFn, pos(4.0, 5.0, 6.0), dir(0.0, 1.0, 0.0));

  QPointer<Point> point = new Point();
  QPointer<FeatureWrapper> pointWrapper = new FeatureWrapper();
  pointWrapper->setPoint(point);
  point->setFeatureName("Plane1.PointOnPlane");
  job->addFeature(pointWrapper);

  QPointer<PointFromPlaneOrigin> function = new PointFromPlaneOrigin();
  function->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, pointWrapper, function, 0));

  point->recalc();

  QVERIFY(point->getOwnerDescription().has_value());
  QVERIFY(point->getOwnerDescription()->contains("Plane1"));

  QCOMPARE(point->getPosition().getVector().getAt(0), 4.0);
  QCOMPARE(point->getPosition().getVector().getAt(1), 5.0);
  QCOMPARE(point->getPosition().getVector().getAt(2), 6.0);
}

void OwnedPlaneAccessorsTest::movingThePlaneRegeneratesItsOwnedAccessors() {
  QPointer<FeatureWrapper> planeWrapper;
  QPointer<SetPlaneFunction> planeFn;
  QPointer<OiJob> job = this->makeJobWithPlane(
      planeWrapper, planeFn, pos(0.0, 0.0, 0.0), dir(0.0, 0.0, 1.0));

  QPointer<Line> line = new Line();
  QPointer<FeatureWrapper> lineWrapper = new FeatureWrapper();
  lineWrapper->setLine(line);
  line->setFeatureName("Plane1.NormalLine");
  job->addFeature(lineWrapper);
  QPointer<LineFromPlaneNormal> lineFn = new LineFromPlaneNormal();
  lineFn->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, lineWrapper, lineFn, 0));

  QPointer<Point> point = new Point();
  QPointer<FeatureWrapper> pointWrapper = new FeatureWrapper();
  pointWrapper->setPoint(point);
  point->setFeatureName("Plane1.PointOnPlane");
  job->addFeature(pointWrapper);
  QPointer<PointFromPlaneOrigin> pointFn = new PointFromPlaneOrigin();
  pointFn->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, pointWrapper, pointFn, 0));

  line->recalc();
  line->setIsUpdated(true);
  point->recalc();
  point->setIsUpdated(true);
  QCOMPARE(point->getPosition().getVector().getAt(0), 0.0);

  // re-measuring the plane must not require touching the accessors by
  // hand - the plane's own recalc dirties them, and the (simulated) DAG
  // walk regenerates them from the new value
  planeFn->position = pos(10.0, 20.0, 30.0);
  planeFn->direction = dir(1.0, 0.0, 0.0);
  planeWrapper->getPlane()->recalc();

  QVERIFY2(!line->getIsUpdated(),
           "moving the plane must dirty its owned normal line");
  QVERIFY2(!point->getIsUpdated(),
           "moving the plane must dirty its owned origin point");

  line->recalc();
  point->recalc();

  QCOMPARE(point->getPosition().getVector().getAt(0), 10.0);
  QCOMPARE(point->getPosition().getVector().getAt(2), 30.0);
  QCOMPARE(line->getDirection().getVector().getAt(0), 1.0);
}

void OwnedPlaneAccessorsTest::editingAGeneratedAccessorIsRejected() {
  QPointer<FeatureWrapper> planeWrapper;
  QPointer<SetPlaneFunction> planeFn;
  QPointer<OiJob> job = this->makeJobWithPlane(
      planeWrapper, planeFn, pos(1.0, 1.0, 1.0), dir(0.0, 0.0, 1.0));

  QPointer<Point> point = new Point();
  QPointer<FeatureWrapper> pointWrapper = new FeatureWrapper();
  pointWrapper->setPoint(point);
  point->setFeatureName("Plane1.PointOnPlane");
  job->addFeature(pointWrapper);
  QPointer<PointFromPlaneOrigin> function = new PointFromPlaneOrigin();
  function->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, pointWrapper, function, 0));

  point->recalc();
  QCOMPARE(point->getPosition().getVector().getAt(0), 1.0);

  // a direct user edit, exactly the entry point the nominal properties
  // dialog uses, must be rejected - it is bound to the plane
  QMap<GeometryParameters, double> edit;
  edit.insert(eUnknownX, 999.0);
  point->setUnknownParameters(edit);

  QCOMPARE(point->getPosition().getVector().getAt(0), 1.0);
}

void OwnedPlaneAccessorsTest::deletingThePlaneDeletesItsOwnedAccessors() {
  QPointer<FeatureWrapper> planeWrapper;
  QPointer<SetPlaneFunction> planeFn;
  QPointer<OiJob> job = this->makeJobWithPlane(
      planeWrapper, planeFn, pos(0.0, 0.0, 0.0), dir(0.0, 0.0, 1.0));

  QPointer<Line> line = new Line();
  QPointer<FeatureWrapper> lineWrapper = new FeatureWrapper();
  lineWrapper->setLine(line);
  line->setFeatureName("Plane1.NormalLine");
  job->addFeature(lineWrapper);
  QPointer<LineFromPlaneNormal> lineFn = new LineFromPlaneNormal();
  lineFn->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, lineWrapper, lineFn, 0));

  QPointer<Point> point = new Point();
  QPointer<FeatureWrapper> pointWrapper = new FeatureWrapper();
  pointWrapper->setPoint(point);
  point->setFeatureName("Plane1.PointOnPlane");
  job->addFeature(pointWrapper);
  QPointer<PointFromPlaneOrigin> pointFn = new PointFromPlaneOrigin();
  pointFn->init();
  QVERIFY(job->addOwnedSubGeometry(planeWrapper, pointWrapper, pointFn, 0));

  const int lineId = line->getId();
  const int pointId = point->getId();
  const int planeId = planeWrapper->getPlane()->getId();

  QVERIFY(job->removeFeature(planeId));

  QVERIFY(job->getFeatureById(lineId).isNull());
  QVERIFY(job->getFeatureById(pointId).isNull());
}

QTEST_APPLESS_MAIN(OwnedPlaneAccessorsTest)
#include "tst_ownedplaneaccessors.moc"
