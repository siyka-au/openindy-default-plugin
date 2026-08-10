#include <QtTest>
#include <cmath>

#include "chooselalib.h"
#include "p_planefrompointcollection.h"
#include "p_averagefrompoints.h"
#include "featurewrapper.h"
#include "geometry/plane.h"
#include "geometry/point.h"

using namespace oi;
using namespace oi::math;

namespace{

QPointer<Point> makePoint(double x, double y, double z, int id){
    QPointer<Point> point = new Point(id, Position(x, y, z));
    point->setIsSolved(true);
    return point;
}

//feed a point into a function's declared input position - what
//OiJob::addInputPoint does once tag resolution (D12) has matched a
//Point's MeasurementIntent to a NeededElement::roleName. Tests at this
//level bypass OiJob/tagging entirely and drive the function directly, the
//same way tst_function.cpp does for every other fit/construct function.
void addInputPoint(QPointer<Function> function, const QPointer<Point> &point, int neededElementsIndex = 0){
    InputElement element(point->getId());
    element.typeOfElement = ePointElement;
    element.point = point;
    function->addInputElement(element, neededElementsIndex);
}

double planeNormalAngleTo(const QPointer<Plane> &plane, double i, double j, double k){

    const OiVec fitted = plane->getDirection().getVector();
    const double fittedNorm = std::sqrt(fitted.getAt(0)*fitted.getAt(0)
                                      + fitted.getAt(1)*fitted.getAt(1)
                                      + fitted.getAt(2)*fitted.getAt(2));
    const double trueNorm = std::sqrt(i*i + j*j + k*k);

    double dot = (fitted.getAt(0)*i + fitted.getAt(1)*j + fitted.getAt(2)*k) / (fittedNorm * trueNorm);
    dot = std::clamp(dot, -1.0, 1.0);
    return std::acos(std::abs(dot)) * 180.0 / M_PI;

}

}

/*!
 * Originally Stage 7a-ii's acceptance criteria for binding a whole
 * PointCollection as a single function input. Stage 7f retires
 * PointCollection as a fit-input mechanism (domain-model.md S5) in favour
 * of tag resolution (D12): a fit's point input resolves to a query over
 * MeasurementIntent tags, not a stored membership list, so its declared
 * shape (NeededElement count) still never grows regardless of point count
 * - the same guarantee PointCollection binding used to provide, delivered
 * differently.
 *
 * Reviewed rather than deleted, per the Stage 7f scope: the two properties
 * that must survive the mechanism change are still asserted here -
 * functionInputListNeverGrowsWithPointCount (now about the *declaration*,
 * since the resolved InputElement list legitimately does grow one per
 * tagged point - that growth is expected, not the friction this design
 * removes) and the "hint not gate" degenerate-input cases. What's gone is
 * pointCloudBindsToTheSameFunction: a PointCloud is explicitly not a Point
 * (domain-model.md S5 - "you can tag fifty V-STARS targets but not 40,000
 * scan points"), so it can no longer satisfy an ePointElement input at
 * all. That is a deliberate behaviour change, not an oversight.
 */
class CollectionBindingTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void pointsAddedAfterInitialResolutionArePickedUp();
    void declaredShapeNeverGrowsWithPointCount();
    void tooFewPointsFailsCleanly();
    void collinearPointsAreRejectedByTheFitNotTheCount();
    void planeFitsIdenticallyFromPointsAveragedByAverageFromPoints();
};

void CollectionBindingTest::initTestCase(){
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

void CollectionBindingTest::pointsAddedAfterInitialResolutionArePickedUp(){

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();

    //three points: enough to solve, all on z = 0
    addInputPoint(function, makePoint(0.0, 0.0, 0.0, 1));
    addInputPoint(function, makePoint(1.0, 0.0, 0.0, 2));
    addInputPoint(function, makePoint(0.0, 1.0, 0.0, 3));

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(function->exec(planeFeature), "plane should solve from three points");
    QVERIFY2(planeNormalAngleTo(plane, 0.0, 0.0, 1.0) < 1.0, "plane should be flat in z");

    //now the part that matters: more points arrive (as if freshly tagged
    //and resolved by OiJob) and the plane re-solves without the function
    //ever being re-declared
    addInputPoint(function, makePoint(2.0, 0.0, 0.5, 4));
    addInputPoint(function, makePoint(0.0, 2.0, 1.0, 5));

    QVERIFY2(function->exec(planeFeature), "plane should re-solve after points were tagged for it");

    //the fit genuinely used the new points - the plane is no longer flat
    QVERIFY2(planeNormalAngleTo(plane, 0.0, 0.0, 1.0) > 1.0,
              "the added points must have changed the fit; if not, they were never seen");

}

void CollectionBindingTest::declaredShapeNeverGrowsWithPointCount(){

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();

    //D12: one declared NeededElement regardless of how many points end up
    //tagged for it - this is what "cannot grow by construction" now means
    QCOMPARE(function->getNeededElements().size(), 1);

    //the TS15-grid case in miniature: a large automated acquisition
    for(int i = 0; i < 500; ++i){
        addInputPoint(function, makePoint(i * 0.1, (i % 7) * 0.1, 0.0, 100 + i));
    }

    //the resolved InputElement list legitimately grows one per point (it
    //always did, even for BestFitPlane's raw observations) - that is not
    //the friction PointCollection binding removed
    QCOMPARE(function->getInputElements().value(0).size(), 500);

    //what must not have changed is the function's own declared shape
    QCOMPARE(function->getNeededElements().size(), 1);

}

void CollectionBindingTest::tooFewPointsFailsCleanly(){

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();

    addInputPoint(function, makePoint(0.0, 0.0, 0.0, 1));
    addInputPoint(function, makePoint(1.0, 0.0, 0.0, 2));

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(!function->exec(planeFeature), "two points cannot define a plane");

}

void CollectionBindingTest::collinearPointsAreRejectedByTheFitNotTheCount(){

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();

    //three points satisfies the declared minimum, and they still cannot
    //define a plane. This is exactly why a minimum count is a hint and the
    //real validation lives in the fit itself.
    addInputPoint(function, makePoint(0.0, 0.0, 0.0, 1));
    addInputPoint(function, makePoint(1.0, 1.0, 1.0, 2));
    addInputPoint(function, makePoint(2.0, 2.0, 2.0, 3));

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(!function->exec(planeFeature),
              "collinear points pass the count check and must still be rejected by the fit");

}

/*!
 * Stage 7f acceptance criterion: "three raw points averaged by
 * AverageFromPoints produce one derived point and the plane fits from
 * derived points identically". Four nests, three raw shots each (D15a's
 * own example, shrunk to fit a unit test) - AverageFromPoints combines
 * each nest's three raw points into one derived point, and the plane
 * fits from the four derived points exactly as it would from four points
 * measured with no repetition at all.
 */
void CollectionBindingTest::planeFitsIdenticallyFromPointsAveragedByAverageFromPoints(){

    //the four "true" nest positions the plane should fit from - flat in z
    struct Nest{ double x, y, z; };
    const QList<Nest> nests = { {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0} };

    int id = 1;
    int derivedId = 1000;
    QList<QPointer<Point> > derivedPoints;

    foreach(const Nest &nest, nests){

        //three raw shots of the same nest, each nudged slightly off the
        //true position - the average should recover it
        QPointer<Function> averager = new AverageFromPoints();
        averager->init();
        addInputPoint(averager, makePoint(nest.x - 0.01, nest.y, nest.z, id++));
        addInputPoint(averager, makePoint(nest.x + 0.01, nest.y, nest.z, id++));
        addInputPoint(averager, makePoint(nest.x, nest.y, nest.z, id++));

        QPointer<Point> derived = new Point(derivedId++, Position());
        QPointer<FeatureWrapper> derivedFeature = new FeatureWrapper();
        derivedFeature->setPoint(derived);

        QVERIFY2(averager->exec(derivedFeature), "three points should average cleanly");
        derived->setIsSolved(true);

        //D15a: the raw shots are individually visible/inspectable - they
        //are not consumed or hidden by averaging
        QCOMPARE(averager->getInputElements().value(0).size(), 3);

        derivedPoints.append(derived);

    }

    //the plane fits from the four derived points
    QPointer<Function> plane1Function = new PlaneFromPointCollection();
    plane1Function->init();
    foreach(const QPointer<Point> &derived, derivedPoints){
        addInputPoint(plane1Function, derived);
    }
    QPointer<Plane> planeFromAveraged = new Plane();
    QPointer<FeatureWrapper> planeFromAveragedFeature = new FeatureWrapper();
    planeFromAveragedFeature->setPlane(planeFromAveraged);
    QVERIFY2(plane1Function->exec(planeFromAveragedFeature), "plane should solve from four averaged points");

    //and identically from the four true, un-repeated positions
    QPointer<Function> plane2Function = new PlaneFromPointCollection();
    plane2Function->init();
    int trueId = 5000;
    foreach(const Nest &nest, nests){
        addInputPoint(plane2Function, makePoint(nest.x, nest.y, nest.z, trueId++));
    }
    QPointer<Plane> planeFromTrue = new Plane();
    QPointer<FeatureWrapper> planeFromTrueFeature = new FeatureWrapper();
    planeFromTrueFeature->setPlane(planeFromTrue);
    QVERIFY2(plane2Function->exec(planeFromTrueFeature), "plane should solve from four true points");

    const double angleBetweenFits = planeNormalAngleTo(planeFromAveraged,
        planeFromTrue->getDirection().getVector().getAt(0),
        planeFromTrue->getDirection().getVector().getAt(1),
        planeFromTrue->getDirection().getVector().getAt(2));
    QVERIFY2(angleBetweenFits < 1.0, "averaging repeated shots first must not change the fitted plane");

}

QTEST_APPLESS_MAIN(CollectionBindingTest)
#include "tst_collectionbinding.moc"
