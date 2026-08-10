#include <QtTest>
#include <cmath>

#include "chooselalib.h"
#include "p_planefrompointcollection.h"
#include "featurewrapper.h"
#include "geometry/plane.h"
#include "geometry/point.h"
#include "geometry/pointcloud.h"
#include "pointgroup.h"

using namespace oi;
using namespace oi::math;

namespace{

QPointer<FeatureWrapper> makePoint(double x, double y, double z, int id){
    QPointer<Point> point = new Point(id, Position(x, y, z));
    point->setIsSolved(true);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPoint(point);
    return wrapper;
}

//bind a collection as the function's single input element, the way OiJob
//does when a user picks a collection for a fit
void bindCollection(QPointer<Function> function, const QPointer<FeatureWrapper> &collectionFeature){

    InputElement element(1);
    element.typeOfElement = ePointCollectionElement;
    element.shouldBeUsed = true;
    element.pointGroup = collectionFeature->getPointGroup();
    element.group = collectionFeature->getGroup();
    element.pointCloud = collectionFeature->getPointCloud();

    function->addInputElement(element, 0);

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
 * Stage 7a-ii's acceptance criteria. The claim being tested is that binding
 * a whole collection, rather than N individual points, actually delivers
 * what it was designed for: bulk acquisition never touches the consuming
 * function, and provenance (scan vs. individually aimed shots) is invisible
 * to the fit.
 */
class CollectionBindingTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void pointsAddedAfterBindingArePickedUp();
    void functionInputListNeverGrowsWithPointCount();
    void pointCloudBindsToTheSameFunction();
    void tooFewPointsFailsCleanly();
    void collinearPointsAreRejectedByTheFitNotTheCount();
};

void CollectionBindingTest::initTestCase(){
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

void CollectionBindingTest::pointsAddedAfterBindingArePickedUp(){

    QPointer<PointGroup> group = new PointGroup("wall");
    QPointer<FeatureWrapper> groupFeature = new FeatureWrapper();
    groupFeature->setPointGroup(group);

    //three points: enough to solve, all on z = 0
    group->addMember(makePoint(0.0, 0.0, 0.0, 1));
    group->addMember(makePoint(1.0, 0.0, 0.0, 2));
    group->addMember(makePoint(0.0, 1.0, 0.0, 3));

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();
    bindCollection(function, groupFeature);

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(function->exec(planeFeature), "plane should solve from three points");
    QVERIFY2(planeNormalAngleTo(plane, 0.0, 0.0, 1.0) < 1.0, "plane should be flat in z");

    //now the part that matters: add points to the group and re-solve
    //WITHOUT touching the function's inputs at all. This is the automated
    //bulk-capture case - a driver appends to the collection and nothing
    //rebinds anything.
    group->addMember(makePoint(2.0, 0.0, 0.5, 4));
    group->addMember(makePoint(0.0, 2.0, 1.0, 5));

    QVERIFY2(function->exec(planeFeature), "plane should re-solve after points were added to the group");

    //the fit genuinely used the new points - the plane is no longer flat
    QVERIFY2(planeNormalAngleTo(plane, 0.0, 0.0, 1.0) > 1.0,
              "the added points must have changed the fit; if not, they were never seen");

}

void CollectionBindingTest::functionInputListNeverGrowsWithPointCount(){

    QPointer<PointGroup> group = new PointGroup("wall");
    QPointer<FeatureWrapper> groupFeature = new FeatureWrapper();
    groupFeature->setPointGroup(group);

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();
    bindCollection(function, groupFeature);

    QCOMPARE(function->getInputElements().value(0).size(), 1);

    //the TS15-grid case in miniature: a large automated acquisition
    for(int i = 0; i < 500; ++i){
        group->addMember(makePoint(i * 0.1, (i % 7) * 0.1, 0.0, 100 + i));
    }

    QCOMPARE(group->memberCount(), 500);

    //500 points, still exactly one input element. Binding points
    //individually would have made this 500 - which is precisely the
    //friction this design removes.
    QCOMPARE(function->getInputElements().value(0).size(), 1);

}

void CollectionBindingTest::pointCloudBindsToTheSameFunction(){

    QPointer<PointCloud> cloud = new PointCloud();
    cloud->addPointCloudPoint(new Point_PC(0.0f, 0.0f, 0.0f));
    cloud->addPointCloudPoint(new Point_PC(1.0f, 0.0f, 0.0f));
    cloud->addPointCloudPoint(new Point_PC(0.0f, 1.0f, 0.0f));
    cloud->addPointCloudPoint(new Point_PC(1.0f, 1.0f, 0.0f));

    QPointer<FeatureWrapper> cloudFeature = new FeatureWrapper();
    cloudFeature->setPointCloud(cloud);

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();
    bindCollection(function, cloudFeature);

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    //identical function, identical binding, different provenance entirely
    QVERIFY2(function->exec(planeFeature), "the same function must accept a point cloud");
    QVERIFY2(planeNormalAngleTo(plane, 0.0, 0.0, 1.0) < 1.0, "plane should be flat in z");

}

void CollectionBindingTest::tooFewPointsFailsCleanly(){

    QPointer<PointGroup> group = new PointGroup("sparse");
    QPointer<FeatureWrapper> groupFeature = new FeatureWrapper();
    groupFeature->setPointGroup(group);

    group->addMember(makePoint(0.0, 0.0, 0.0, 1));
    group->addMember(makePoint(1.0, 0.0, 0.0, 2));

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();
    bindCollection(function, groupFeature);

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(!function->exec(planeFeature), "two points cannot define a plane");

}

void CollectionBindingTest::collinearPointsAreRejectedByTheFitNotTheCount(){

    QPointer<PointGroup> group = new PointGroup("collinear");
    QPointer<FeatureWrapper> groupFeature = new FeatureWrapper();
    groupFeature->setPointGroup(group);

    //three points satisfies the declared minimum, and they still cannot
    //define a plane. This is exactly why a minimum count is a hint and the
    //real validation lives in the fit itself.
    group->addMember(makePoint(0.0, 0.0, 0.0, 1));
    group->addMember(makePoint(1.0, 1.0, 1.0, 2));
    group->addMember(makePoint(2.0, 2.0, 2.0, 3));

    QCOMPARE(group->collectionPointCount(), 3);

    QPointer<Function> function = new PlaneFromPointCollection();
    function->init();
    bindCollection(function, groupFeature);

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(!function->exec(planeFeature),
              "collinear points pass the count check and must still be rejected by the fit");

}

QTEST_APPLESS_MAIN(CollectionBindingTest)
#include "tst_collectionbinding.moc"
