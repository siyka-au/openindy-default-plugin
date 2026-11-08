#include <QtTest>
#include <cmath>

#include "choose_la_lib.h"
#include "feature_wrapper.h"
#include "geometry/line.h"
#include "geometry/plane.h"
#include "geometry/point.h"
#include "p_line_from_plane_normal.h"
#include "p_point_from_plane_origin.h"

using namespace oi;
using namespace oi::math;

namespace {

oi::Guarded<FeatureWrapper> makePlane(const Position& position,
                                      const Direction& normal) {
  oi::Guarded<Plane> plane = new Plane();
  Position p = position;
  Direction n = normal;
  plane->setPlane(p, n);
  plane->setIsSolved(true);
  oi::Guarded<oi::FeatureWrapper> wrapper = plane->getFeatureWrapper();
  return wrapper;
}

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

void bindPlane(oi::Guarded<Function> function,
               const oi::Guarded<FeatureWrapper>& planeFeature) {
  InputElement element(1);
  element.typeOfElement = ElementTypes::ePlaneElement;
  element.shouldBeUsed = true;
  element.plane = planeFeature->getPlane();
  element.geometry = planeFeature->getGeometry();
  function->addInputElement(element, 0);
}

}  // namespace

/*!
 * Stage 7c-i. D8's position in practice: derived geometry comes from a
 * declared function on a feature the user created, never from a fit
 * spontaneously emitting extra features.
 *
 * The property that matters is that these are live derivations, not
 * one-off copies - re-measure the plane and the derived line and point
 * follow. That is the thing that removes the manual backfilling other
 * packages force, and it needs no new machinery: the existing dependency
 * graph already re-executes a feature's functions.
 */
class DerivedFeaturesTest : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase();

  void lineTakesPlaneNormalAndPosition();
  void pointTakesPlaneOrigin();
  void derivedFeaturesFollowTheirSourcePlane();
  void unsolvedPlaneYieldsNoDerivedFeature();
};

void DerivedFeaturesTest::initTestCase() {
  ChooseLALib::setLinearAlgebra(ChooseLALib::LinearAlgebraLibrary::Armadillo);
}

void DerivedFeaturesTest::lineTakesPlaneNormalAndPosition() {
  oi::Guarded<FeatureWrapper> planeFeature =
      makePlane(pos(1.0, 2.0, 3.0), dir(0.0, 0.0, 1.0));

  std::unique_ptr<Function> owned_function =
      std::make_unique<LineFromPlaneNormal>();

  oi::Guarded<Function> function(owned_function.get());
  function->init();
  bindPlane(function, planeFeature);

  oi::Guarded<Line> line = new Line();
  oi::Guarded<oi::FeatureWrapper> lineFeature = line->getFeatureWrapper();

  QVERIFY(function->exec(lineFeature));

  QCOMPARE(line->getPosition().getVector().getAt(0), 1.0);
  QCOMPARE(line->getPosition().getVector().getAt(2), 3.0);
  QCOMPARE(line->getDirection().getVector().getAt(2), 1.0);
}

void DerivedFeaturesTest::pointTakesPlaneOrigin() {
  oi::Guarded<FeatureWrapper> planeFeature =
      makePlane(pos(4.0, 5.0, 6.0), dir(0.0, 1.0, 0.0));

  std::unique_ptr<Function> owned_function =
      std::make_unique<PointFromPlaneOrigin>();

  oi::Guarded<Function> function(owned_function.get());
  function->init();
  bindPlane(function, planeFeature);

  oi::Guarded<Point> point = new Point();
  oi::Guarded<oi::FeatureWrapper> pointFeature = point->getFeatureWrapper();

  QVERIFY(function->exec(pointFeature));

  QCOMPARE(point->getPosition().getVector().getAt(0), 4.0);
  QCOMPARE(point->getPosition().getVector().getAt(1), 5.0);
  QCOMPARE(point->getPosition().getVector().getAt(2), 6.0);
}

void DerivedFeaturesTest::derivedFeaturesFollowTheirSourcePlane() {
  oi::Guarded<FeatureWrapper> planeFeature =
      makePlane(pos(0.0, 0.0, 0.0), dir(0.0, 0.0, 1.0));

  std::unique_ptr<Function> owned_lineFunction =
      std::make_unique<LineFromPlaneNormal>();

  oi::Guarded<Function> lineFunction(owned_lineFunction.get());
  lineFunction->init();
  bindPlane(lineFunction, planeFeature);

  std::unique_ptr<Function> owned_pointFunction =
      std::make_unique<PointFromPlaneOrigin>();

  oi::Guarded<Function> pointFunction(owned_pointFunction.get());
  pointFunction->init();
  bindPlane(pointFunction, planeFeature);

  oi::Guarded<Line> line = new Line();
  oi::Guarded<oi::FeatureWrapper> lineFeature = line->getFeatureWrapper();

  oi::Guarded<Point> point = new Point();
  oi::Guarded<oi::FeatureWrapper> pointFeature = point->getFeatureWrapper();

  QVERIFY(lineFunction->exec(lineFeature));
  QVERIFY(pointFunction->exec(pointFeature));
  QCOMPARE(point->getPosition().getVector().getAt(0), 0.0);

  // the plane is re-measured and lands somewhere else, tilted
  Position moved = pos(10.0, 20.0, 30.0);
  Direction tilted = dir(1.0, 0.0, 0.0);
  planeFeature->getPlane()->setPlane(moved, tilted);

  // re-executing is all it takes - no rebinding, no manual update. This is
  // what the existing dependency graph does on recalc.
  QVERIFY(lineFunction->exec(lineFeature));
  QVERIFY(pointFunction->exec(pointFeature));

  QCOMPARE(point->getPosition().getVector().getAt(0), 10.0);
  QCOMPARE(point->getPosition().getVector().getAt(2), 30.0);
  QCOMPARE(line->getDirection().getVector().getAt(0), 1.0);
}

void DerivedFeaturesTest::unsolvedPlaneYieldsNoDerivedFeature() {
  // an unsolved plane has no trustworthy normal or position, so deriving
  // from it would invent geometry that was never measured
  oi::Guarded<Plane> plane = new Plane();
  plane->setIsSolved(false);
  oi::Guarded<oi::FeatureWrapper> planeFeature = plane->getFeatureWrapper();

  std::unique_ptr<Function> owned_function =
      std::make_unique<LineFromPlaneNormal>();

  oi::Guarded<Function> function(owned_function.get());
  function->init();
  bindPlane(function, planeFeature);

  oi::Guarded<Line> line = new Line();
  oi::Guarded<oi::FeatureWrapper> lineFeature = line->getFeatureWrapper();

  QVERIFY2(!function->exec(lineFeature),
           "an unsolved plane must not produce a derived line");
}

QTEST_APPLESS_MAIN(DerivedFeaturesTest)
#include "tst_derived_features.moc"
