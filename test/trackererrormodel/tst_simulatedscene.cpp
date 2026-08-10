#include <QtTest>
#include <cmath>

#include "chooselalib.h"
#include "simulatedobservation.h"
#include "p_bestfitplane.h"
#include "featurewrapper.h"
#include "geometry/plane.h"
#include "oivec.h"
#include "reading.h"
#include "observation.h"

using namespace oi;
using namespace oi::math;

class SimulatedSceneTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    //the Stage 2 acceptance criterion: a simulated instrument observing a
    //known scene produces observations that (a) let an existing fit function
    //converge to the true geometry within the declared error budget, and
    //(b) carry sigma that is actually populated and of a sane magnitude -
    //not the zero/default that Observation::sigmaXyz silently returned
    //before this stage
    void trackerObservedPlaneConvergesWithinErrorBudget();

    //same scene, same fit, a second, independently-configured error model -
    //this is "prove the split works": TrackerErrorModel is not hardcoded to
    //PseudoTracker's specific numbers, a differently-toleranced instrument
    //(here: reflector vs. reflectorless on the same simulated total
    //station, which really do differ - a total station can be markedly
    //less precise reflectorless, and loses ATR/target-lock entirely) is
    //just a different TrackerErrorTerms value
    void reflectorlessObservationIsNoisierThanReflector();
};

void SimulatedSceneTest::initTestCase(){
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

void SimulatedSceneTest::trackerObservedPlaneConvergesWithinErrorBudget(){

    //a station 8m back from a slightly tilted 2x2m panel - representative
    //working distance for a laser tracker, not a toy close-range case
    StationPose pose;
    pose.position.setAt(0, 0.0);
    pose.position.setAt(1, 0.0);
    pose.position.setAt(2, 0.0);
    pose.orientation = OiMat(3, 3);
    pose.orientation.setAt(0, 0, 1.0);
    pose.orientation.setAt(1, 1, 1.0);
    pose.orientation.setAt(2, 2, 1.0);

    QList<OiVec> scene;
    auto point = [](double x, double y, double z){
        OiVec v(3);
        v.setAt(0, x); v.setAt(1, y); v.setAt(2, z);
        return v;
    };
    //z = 0.01*x + 0.02*y : a real, non-axis-aligned plane, not a flat one
    //a fit could satisfy by accident
    auto onPlane = [](double x, double y){ return 0.01*x + 0.02*y; };
    scene << point(8.0, -1.0, onPlane(8.0, -1.0))
          << point(9.0,  1.0, onPlane(9.0,  1.0))
          << point(9.0, -1.0, onPlane(9.0, -1.0))
          << point(8.0,  1.0, onPlane(8.0,  1.0))
          << point(8.5,  0.0, onPlane(8.5,  0.0));

    //PseudoTracker's real, deployed error terms - not invented for this test
    TrackerErrorTerms terms;
    terms.lambdaMm = 0.000403; terms.mu = 0.000005; terms.exMm = 0.0000122;
    terms.byMm = 0.0000654; terms.bzMm = 0.0000974;
    terms.alphaArcsec = 0.128; terms.gammaArcsec = 0.079;
    terms.Aa1Arcsec = 0.064; terms.Ba1Arcsec = 0.080;
    terms.Aa2Arcsec = 0.073; terms.Ba2Arcsec = 0.090;
    terms.Ae0Arcsec = 0.223; terms.Ae1Arcsec = 0.152; terms.Be1Arcsec = 0.183;
    terms.Ae2Arcsec = 0.214; terms.Be2Arcsec = 0.179;
    const TrackerErrorModel model(terms);

    QPointer<Function> function = new BestFitPlane();
    function->init();

    int id = 1;
    QList<QPointer<Observation>> observations;
    for(const OiVec &truePoint : scene){
        QPointer<Observation> obs = simulateObservation(truePoint, pose, model, id++);
        observations.append(obs);
        QVERIFY2(obs->getSigmaXYZ().getAt(0) > 0.0, "sigma must be populated, not the silent zero default");

        InputElement element(obs->getId());
        element.typeOfElement = eObservationElement;
        element.observation = obs;
        element.shouldBeUsed = true;
        function->addInputElement(element, InputElementKey::eDefault);
    }

    QPointer<Plane> plane = new Plane();
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    QVERIFY2(function->exec(planeFeature), "plane fit did not converge");

    //true normal for z = 0.01x + 0.02y, i.e. -0.01x - 0.02y + z = 0
    OiVec trueNormal(3);
    trueNormal.setAt(0, -0.01);
    trueNormal.setAt(1, -0.02);
    trueNormal.setAt(2, 1.0);
    const double trueNorm = std::sqrt(trueNormal.getAt(0)*trueNormal.getAt(0)
                                     + trueNormal.getAt(1)*trueNormal.getAt(1)
                                     + trueNormal.getAt(2)*trueNormal.getAt(2));

    const OiVec fittedNormal = plane->getDirection().getVector();
    const double fittedNorm = std::sqrt(fittedNormal.getAt(0)*fittedNormal.getAt(0)
                                       + fittedNormal.getAt(1)*fittedNormal.getAt(1)
                                       + fittedNormal.getAt(2)*fittedNormal.getAt(2));

    double dot = (trueNormal.getAt(0)*fittedNormal.getAt(0)
                + trueNormal.getAt(1)*fittedNormal.getAt(1)
                + trueNormal.getAt(2)*fittedNormal.getAt(2)) / (trueNorm * fittedNorm);
    dot = std::clamp(dot, -1.0, 1.0);
    const double angleErrorArcsec = std::acos(std::abs(dot)) * (648000.0 / M_PI);

    //loose but meaningful: converges to within 60 arcsec of true, comfortably
    //inside what a plane fit should achieve for ~0.1-0.2 arcsec-class
    //per-point angular noise averaged over 5 points - failing this means
    //the fit diverged, not that the tolerance is merely tight
    QVERIFY2(angleErrorArcsec < 60.0,
              qPrintable(QString("fitted plane normal off by %1 arcsec").arg(angleErrorArcsec)));

}

void SimulatedSceneTest::reflectorlessObservationIsNoisierThanReflector(){

    StationPose pose;
    pose.position = OiVec(3);
    pose.orientation = OiMat(3, 3);
    pose.orientation.setAt(0, 0, 1.0);
    pose.orientation.setAt(1, 1, 1.0);
    pose.orientation.setAt(2, 2, 1.0);

    OiVec target(3);
    target.setAt(0, 20.0);
    target.setAt(1, 2.0);
    target.setAt(2, 0.5);

    //same instrument, two modes - reflectorless is a materially worse
    //distance term on a real total station (D4: this is true even within
    //one instrument, not just between instrument tiers)
    TrackerErrorTerms reflectorTerms;
    reflectorTerms.lambdaMm = 1.0;   //~1mm-class fixed term
    reflectorTerms.mu = 0.0000015;   //~1.5ppm-class scale term
    reflectorTerms.Aa1Arcsec = 1.0;
    reflectorTerms.Ae1Arcsec = 1.0;

    TrackerErrorTerms reflectorlessTerms = reflectorTerms;
    reflectorlessTerms.lambdaMm = 2.0;  //~2mm-class fixed term
    reflectorlessTerms.mu = 0.000002;   //~2ppm-class scale term

    const TrackerErrorModel reflectorModel(reflectorTerms, 400);
    const TrackerErrorModel reflectorlessModel(reflectorlessTerms, 400);

    const ReadingPolar trueReading = trueObservationOf(target, pose);

    const ReadingPolar reflectorReading = reflectorModel.apply(trueReading.azimuth, trueReading.zenith, trueReading.distance);
    const ReadingPolar reflectorlessReading = reflectorlessModel.apply(trueReading.azimuth, trueReading.zenith, trueReading.distance);

    QVERIFY2(reflectorlessReading.sigmaDistance > reflectorReading.sigmaDistance,
              "reflectorless must be reported as less accurate than reflector on the same instrument");

}

QTEST_APPLESS_MAIN(SimulatedSceneTest)
#include "tst_simulatedscene.moc"
