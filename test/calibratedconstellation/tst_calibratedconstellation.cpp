#include <QtTest>
#include <cmath>

#include "chooselalib.h"
#include "simulatedobservation.h"
#include "trackererrormodel.h"
#include "calibratedconstellation.h"
#include "p_helmert6param.h"
#include "trafoparam.h"
#include "featurewrapper.h"
#include "point.h"
#include "position.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;
using namespace oi::math;

namespace{

//a 3x3 rotation matrix, world<-local, built from a Z rotation followed by
//an X rotation (arbitrary but fixed convention - only used to build a known
//ground-truth pose for this test, not asserted as *the* convention)
OiMat rotationZX(double zDeg, double xDeg){

    const double z = zDeg * M_PI / 180.0;
    const double x = xDeg * M_PI / 180.0;

    OiMat rz(3, 3);
    rz.setAt(0, 0, std::cos(z)); rz.setAt(0, 1, -std::sin(z)); rz.setAt(0, 2, 0.0);
    rz.setAt(1, 0, std::sin(z)); rz.setAt(1, 1,  std::cos(z)); rz.setAt(1, 2, 0.0);
    rz.setAt(2, 0, 0.0);         rz.setAt(2, 1, 0.0);          rz.setAt(2, 2, 1.0);

    OiMat rx(3, 3);
    rx.setAt(0, 0, 1.0); rx.setAt(0, 1, 0.0);          rx.setAt(0, 2, 0.0);
    rx.setAt(1, 0, 0.0); rx.setAt(1, 1, std::cos(x));  rx.setAt(1, 2, -std::sin(x));
    rx.setAt(2, 0, 0.0); rx.setAt(2, 1, std::sin(x));  rx.setAt(2, 2,  std::cos(x));

    return rz * rx;

}

//local-frame point -> world-frame point, given the board's true pose
OiVec toWorld(const OiVec &localXyz, const OiMat &boardRotation, const OiVec &boardTranslation){

    const OiVec rotated = boardRotation * localXyz;

    OiVec world(3);
    world.setAt(0, rotated.getAt(0) + boardTranslation.getAt(0));
    world.setAt(1, rotated.getAt(1) + boardTranslation.getAt(1));
    world.setAt(2, rotated.getAt(2) + boardTranslation.getAt(2));
    return world;

}

OiVec xyz(double x, double y, double z){
    OiVec v(3);
    v.setAt(0, x); v.setAt(1, y); v.setAt(2, z);
    return v;
}

}

/*!
 * Stage 4's acceptance criterion: a simulated total station observes a
 * synthetic 7-point calibration board; the solved pose matches ground
 * truth within propagated uncertainty, and a point on the board that was
 * never itself observed - only known through its fixed relationship to the
 * others - is recovered through that same solved pose. That last part is
 * the entire point of CalibratedConstellation: it is what makes a 7-point
 * board a genuinely different data model from a bag of independently
 * measured points.
 *
 * Pose solving reuses Helmert6Param exactly as it exists today (SVD
 * best-fit rigid transform, D6/Stage 4 design note "nothing new to build")
 * - this test exercises that claim rather than adding new solving code.
 */
class CalibratedConstellationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void solvedPoseRecoversBoardAndDerivedPoint();

};

void CalibratedConstellationTest::initTestCase(){
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

void CalibratedConstellationTest::solvedPoseRecoversBoardAndDerivedPoint(){

    //--- the constellation: 6 measured targets on a calibration board plus
    //one derived fiducial that stands proud of the board plane (like a
    //vector bar's tip) and is never itself observed. Irregular spacing on
    //purpose - a symmetric grid admits more than one rotation that fits
    //the same measured points equally well.
    CalibratedConstellation board("7-point calibration board",
        QList<LocalPoint>()
            << LocalPoint("A1", xyz(0.00, 0.00, 0.00), true)
            << LocalPoint("A2", xyz(0.20, 0.00, 0.00), true)
            << LocalPoint("A3", xyz(0.45, 0.00, 0.00), true)
            << LocalPoint("A4", xyz(0.00, 0.30, 0.00), true)
            << LocalPoint("A5", xyz(0.20, 0.30, 0.00), true)
            << LocalPoint("A6", xyz(0.45, 0.30, 0.00), true)
            << LocalPoint("FID", xyz(0.225, 0.15, 0.05), false),
        "CAL-0001", QDate(2026, 1, 1));

    QCOMPARE(board.measuredPoints().size(), 6);
    QCOMPARE(board.derivedPoints().size(), 1);
    QVERIFY(board.isValid());

    //--- the board's true pose in job/world coordinates - picked up and
    //placed somewhere in free space in front of the station, not at the
    //origin and not axis-aligned, so a bug that only works for an identity
    //pose can't pass by accident
    const OiMat trueBoardRotation = rotationZX(25.0, 8.0);
    const OiVec trueBoardTranslation = xyz(1.2, 0.6, 1.5);

    //--- the observing station - a simulated total station a few metres
    //back, looking roughly at the board
    StationPose station;
    station.position = xyz(0.0, 0.0, 1.0);
    station.orientation = OiMat(3, 3);
    station.orientation.setAt(0, 0, 1.0);
    station.orientation.setAt(1, 1, 1.0);
    station.orientation.setAt(2, 2, 1.0);

    //TS15-shaped: ~1" angular, ~1mm+1.5ppm reflector-class distance terms
    TrackerErrorTerms terms;
    terms.lambdaMm = 1.0;
    terms.mu = 0.0000015;
    terms.Aa1Arcsec = 1.0;
    terms.Ae1Arcsec = 1.0;
    const TrackerErrorModel model(terms);

    //--- observe every measured point, pairing its known local coordinate
    //with its noisy world-frame observation
    QList<Point> localFramePoints;
    QList<Point> observedWorldPoints;

    int id = 1;
    for(const LocalPoint &local : board.measuredPoints()){

        const OiVec trueWorld = toWorld(local.position(), trueBoardRotation, trueBoardTranslation);
        QPointer<Observation> observation = simulateObservation(trueWorld, station, model, id++);

        Point localPoint(false, Position(local.position()));
        localFramePoints.append(localPoint);

        Point observedPoint(false, Position(observation->getXYZ()));
        observedWorldPoints.append(observedPoint);

    }

    //--- solve: known local coordinates as the start system, noisy world
    //observations as the destination system - exactly the "known points in
    //one frame + observed counterparts in another -> rigid transform"
    //problem Helmert6Param already solves
    QPointer<Helmert6Param> solver = new Helmert6Param();
    solver->init();

    ScalarInputParams params;
    params.isValid = true;
    params.stringParameter["calculate scale"] = "no";
    solver->setScalarInputParams(params);

    solver->setInputPoint(localFramePoints, observedWorldPoints);

    QPointer<TrafoParam> trafoParam = new TrafoParam();
    QPointer<FeatureWrapper> trafoFeature = new FeatureWrapper();
    trafoFeature->setTrafoParam(trafoParam);

    //Helmert6Param/SystemTransformation both redeclare exec(TrafoParam&) as
    //protected (the "methods that cannot be reimplemented" block in
    //SystemTransformation), which hides Function's public
    //exec(QPointer<FeatureWrapper>&) for anything typed as Helmert6Param* -
    //go through Function*, same as tst_simulatedscene.cpp does for
    //BestFitPlane
    QPointer<Function> execTarget = solver;
    QVERIFY2(execTarget->exec(trafoFeature), "pose solve did not converge");

    const double stdevMeters = trafoParam->getStatistic().getStdev();
    QVERIFY2(stdevMeters > 0.0, "TrafoParam's residual-based Statistic must be populated - it comes for free from calc_6p");
    QVERIFY2(stdevMeters < 0.01, qPrintable(QString("solved pose residual stdev implausibly large: %1 m").arg(stdevMeters)));

    //tolerance derived from the solve's own reported uncertainty, not a
    //fixed constant - a tighter or looser error model should tighten or
    //loosen this test automatically
    const double toleranceMeters = std::max(5.0 * stdevMeters, 0.001);

    //--- the measured points themselves must land close to their true
    //world positions when carried through the solved transform
    const OiMat &solved = trafoParam->getHomogenMatrix();

    auto applySolved = [&solved](const OiVec &localXyz){
        OiVec h(4);
        h.setAt(0, localXyz.getAt(0));
        h.setAt(1, localXyz.getAt(1));
        h.setAt(2, localXyz.getAt(2));
        h.setAt(3, 1.0);
        return solved * h;
    };

    auto distance = [](const OiVec &a, const OiVec &b){
        const double dx = a.getAt(0) - b.getAt(0);
        const double dy = a.getAt(1) - b.getAt(1);
        const double dz = a.getAt(2) - b.getAt(2);
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    };

    for(const LocalPoint &local : board.measuredPoints()){
        const OiVec trueWorld = toWorld(local.position(), trueBoardRotation, trueBoardTranslation);
        const OiVec recovered = applySolved(local.position());
        const double error = distance(trueWorld, recovered);
        QVERIFY2(error < toleranceMeters,
                  qPrintable(QString("measured point %1 off by %2 m (tolerance %3 m)")
                             .arg(local.label()).arg(error).arg(toleranceMeters)));
    }

    //--- the real point of the whole exercise: the derived point was never
    //observed by the station at all. Its recovery comes entirely from the
    //solved rigid transform applied to its known local-frame coordinate.
    const std::optional<LocalPoint> derived = board.pointByLabel("FID");
    QVERIFY(derived.has_value());
    QVERIFY(!derived->isMeasured());

    const OiVec trueDerivedWorld = toWorld(derived->position(), trueBoardRotation, trueBoardTranslation);
    const OiVec recoveredDerivedWorld = applySolved(derived->position());
    const double derivedError = distance(trueDerivedWorld, recoveredDerivedWorld);

    QVERIFY2(derivedError < toleranceMeters,
              qPrintable(QString("derived (never-observed) point off by %1 m (tolerance %2 m)")
                         .arg(derivedError).arg(toleranceMeters)));

}

QTEST_APPLESS_MAIN(CalibratedConstellationTest)
#include "tst_calibratedconstellation.moc"
