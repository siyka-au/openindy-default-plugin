#include <QtTest>
#include <cmath>

#include "chooselalib.h"
#include "p_linefromplanenormal.h"
#include "p_pointfromplaneorigin.h"
#include "featurewrapper.h"
#include "geometry/plane.h"
#include "geometry/line.h"
#include "geometry/point.h"

using namespace oi;
using namespace oi::math;

namespace{

QPointer<FeatureWrapper> makePlane(const Position &position, const Direction &normal){
    QPointer<Plane> plane = new Plane();
    Position p = position;
    Direction n = normal;
    plane->setPlane(p, n);
    plane->setIsSolved(true);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPlane(plane);
    return wrapper;
}

Position pos(double x, double y, double z){
    Position p;
    p.setVector(x, y, z);
    return p;
}

Direction dir(double i, double j, double k){
    Direction d;
    OiVec v(3);
    v.setAt(0, i); v.setAt(1, j); v.setAt(2, k);
    d.setVector(v);
    return d;
}

void bindPlane(QPointer<Function> function, const QPointer<FeatureWrapper> &planeFeature){
    InputElement element(1);
    element.typeOfElement = ePlaneElement;
    element.shouldBeUsed = true;
    element.plane = planeFeature->getPlane();
    element.geometry = planeFeature->getGeometry();
    function->addInputElement(element, 0);
}

}

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
class DerivedFeaturesTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void lineTakesPlaneNormalAndPosition();
    void pointTakesPlaneOrigin();
    void derivedFeaturesFollowTheirSourcePlane();
    void unsolvedPlaneYieldsNoDerivedFeature();
};

void DerivedFeaturesTest::initTestCase(){
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

void DerivedFeaturesTest::lineTakesPlaneNormalAndPosition(){

    QPointer<FeatureWrapper> planeFeature = makePlane(pos(1.0, 2.0, 3.0), dir(0.0, 0.0, 1.0));

    QPointer<Function> function = new LineFromPlaneNormal();
    function->init();
    bindPlane(function, planeFeature);

    QPointer<Line> line = new Line();
    QPointer<FeatureWrapper> lineFeature = new FeatureWrapper();
    lineFeature->setLine(line);

    QVERIFY(function->exec(lineFeature));

    QCOMPARE(line->getPosition().getVector().getAt(0), 1.0);
    QCOMPARE(line->getPosition().getVector().getAt(2), 3.0);
    QCOMPARE(line->getDirection().getVector().getAt(2), 1.0);

}

void DerivedFeaturesTest::pointTakesPlaneOrigin(){

    QPointer<FeatureWrapper> planeFeature = makePlane(pos(4.0, 5.0, 6.0), dir(0.0, 1.0, 0.0));

    QPointer<Function> function = new PointFromPlaneOrigin();
    function->init();
    bindPlane(function, planeFeature);

    QPointer<Point> point = new Point();
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);

    QVERIFY(function->exec(pointFeature));

    QCOMPARE(point->getPosition().getVector().getAt(0), 4.0);
    QCOMPARE(point->getPosition().getVector().getAt(1), 5.0);
    QCOMPARE(point->getPosition().getVector().getAt(2), 6.0);

}

void DerivedFeaturesTest::derivedFeaturesFollowTheirSourcePlane(){

    QPointer<FeatureWrapper> planeFeature = makePlane(pos(0.0, 0.0, 0.0), dir(0.0, 0.0, 1.0));

    QPointer<Function> lineFunction = new LineFromPlaneNormal();
    lineFunction->init();
    bindPlane(lineFunction, planeFeature);

    QPointer<Function> pointFunction = new PointFromPlaneOrigin();
    pointFunction->init();
    bindPlane(pointFunction, planeFeature);

    QPointer<Line> line = new Line();
    QPointer<FeatureWrapper> lineFeature = new FeatureWrapper();
    lineFeature->setLine(line);

    QPointer<Point> point = new Point();
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);

    QVERIFY(lineFunction->exec(lineFeature));
    QVERIFY(pointFunction->exec(pointFeature));
    QCOMPARE(point->getPosition().getVector().getAt(0), 0.0);

    //the plane is re-measured and lands somewhere else, tilted
    Position moved = pos(10.0, 20.0, 30.0);
    Direction tilted = dir(1.0, 0.0, 0.0);
    planeFeature->getPlane()->setPlane(moved, tilted);

    //re-executing is all it takes - no rebinding, no manual update. This is
    //what the existing dependency graph does on recalc.
    QVERIFY(lineFunction->exec(lineFeature));
    QVERIFY(pointFunction->exec(pointFeature));

    QCOMPARE(point->getPosition().getVector().getAt(0), 10.0);
    QCOMPARE(point->getPosition().getVector().getAt(2), 30.0);
    QCOMPARE(line->getDirection().getVector().getAt(0), 1.0);

}

void DerivedFeaturesTest::unsolvedPlaneYieldsNoDerivedFeature(){

    //an unsolved plane has no trustworthy normal or position, so deriving
    //from it would invent geometry that was never measured
    QPointer<Plane> plane = new Plane();
    plane->setIsSolved(false);
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QPointer<Function> function = new LineFromPlaneNormal();
    function->init();
    bindPlane(function, planeFeature);

    QPointer<Line> line = new Line();
    QPointer<FeatureWrapper> lineFeature = new FeatureWrapper();
    lineFeature->setLine(line);

    QVERIFY2(!function->exec(lineFeature), "an unsolved plane must not produce a derived line");

}

QTEST_APPLESS_MAIN(DerivedFeaturesTest)
#include "tst_derivedfeatures.moc"
