#include <QtTest>
#include <cmath>
#include <numeric>
#include <vector>

#include "chooselalib.h"
#include "trackererrormodel.h"

class TrackerErrorModelTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    //the actual acceptance criterion for Stage 2: a driver using this model
    //can trust the sigma it reports - it must agree with the scatter an
    //independent, external resampling of the same model actually produces
    void reportedSigmaMatchesExternalScatter();

    //zero-width terms must not fabricate noise or a nonzero sigma
    void zeroErrorTermsAreExact();
};

void TrackerErrorModelTest::initTestCase(){
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);
}

void TrackerErrorModelTest::reportedSigmaMatchesExternalScatter(){

    TrackerErrorTerms terms;
    terms.lambdaMm = 0.4;
    terms.mu = 0.000005;
    terms.exMm = 0.01;
    terms.byMm = 0.06;
    terms.bzMm = 0.09;
    terms.alphaArcsec = 0.13;
    terms.gammaArcsec = 0.08;
    terms.Aa1Arcsec = 0.06;
    terms.Ba1Arcsec = 0.08;
    terms.Aa2Arcsec = 0.07;
    terms.Ba2Arcsec = 0.09;
    terms.Ae0Arcsec = 0.22;
    terms.Ae1Arcsec = 0.15;
    terms.Be1Arcsec = 0.18;
    terms.Ae2Arcsec = 0.21;
    terms.Be2Arcsec = 0.18;

    const TrackerErrorModel model(terms, 256);

    const double trueAz = 0.7;
    const double trueZe = 1.1;
    const double trueDist = 12.5;

    //what apply() itself reports for a single call
    const ReadingPolar reported = model.apply(trueAz, trueZe, trueDist);
    QVERIFY(reported.sigmaDistance > 0.0);
    QVERIFY(reported.sigmaAzimuth > 0.0);
    QVERIFY(reported.sigmaZenith > 0.0);

    //an independent external resample of many apply() calls - if apply()'s
    //internal sigma estimate is honest, the scatter of many *actual* draws
    //should land within a normal margin of that reported sigma
    const int externalSamples = 400;
    std::vector<double> distances;
    distances.reserve(externalSamples);
    for(int i = 0; i < externalSamples; i++){
        distances.push_back(model.apply(trueAz, trueZe, trueDist).distance);
    }

    const double mean = std::accumulate(distances.begin(), distances.end(), 0.0) / distances.size();
    double sumSq = 0.0;
    for(const double d : distances){
        sumSq += (d - mean) * (d - mean);
    }
    const double externalSigma = std::sqrt(sumSq / (distances.size() - 1));

    //loose tolerance - this is comparing two independent Monte Carlo
    //estimates of the same underlying distribution, not asserting an exact
    //closed-form result. A factor-of-two-ish band is what actually
    //distinguishes "reporting real sigma" from "reporting a stale/unrelated
    //default", which is the bug this replaces.
    QVERIFY2(externalSigma > reported.sigmaDistance * 0.4 && externalSigma < reported.sigmaDistance * 2.5,
              qPrintable(QString("external sigma %1 vs reported %2").arg(externalSigma).arg(reported.sigmaDistance)));

}

void TrackerErrorModelTest::zeroErrorTermsAreExact(){

    const TrackerErrorTerms terms; //all zero
    const TrackerErrorModel model(terms, 32);

    const ReadingPolar reading = model.apply(0.3, 1.2, 8.0);

    QCOMPARE(reading.sigmaAzimuth, 0.0);
    QCOMPARE(reading.sigmaZenith, 0.0);
    QCOMPARE(reading.sigmaDistance, 0.0);
    QVERIFY(qAbs(reading.distance - 8.0) < 1e-9);
    QVERIFY(qAbs(reading.azimuth - 0.3) < 1e-9);
    QVERIFY(qAbs(reading.zenith - 1.2) < 1e-9);

}

QTEST_APPLESS_MAIN(TrackerErrorModelTest)
#include "tst_trackererrormodel.moc"
