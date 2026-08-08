#include "trackererrormodel.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <random>

#include "oimat.h"
#include "oivec.h"

using namespace oi::math;

namespace{

constexpr double arcsecToRad = M_PI / 648000.0;

std::mt19937 &rng(){
    static thread_local std::mt19937 generator{std::random_device{}()};
    return generator;
}

double normal(double oneSigma){
    if(oneSigma == 0.0){
        return 0.0;
    }
    std::normal_distribution<double> dist(0.0, oneSigma);
    return dist(rng());
}

double stddev(const std::vector<double> &values){
    if(values.size() < 2){
        return 0.0;
    }
    const double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sumSq = 0.0;
    for(const double v : values){
        sumSq += (v - mean) * (v - mean);
    }
    return std::sqrt(sumSq / (values.size() - 1));
}

}

TrackerErrorModel::TrackerErrorModel(const TrackerErrorTerms &oneSigmaTerms, int sigmaEstimationSamples)
    : terms(oneSigmaTerms), samples(std::max(sigmaEstimationSamples, 2)){
}

/*!
 * \brief TrackerErrorModel::sampleOnce
 * The Hughes error model itself, moved verbatim (in structure - the random
 * draws now come from <random> instead of rand()) out of
 * PseudoTracker::noisyPolarReading, which duplicated this identically to
 * SimplePolarMeasurement::distortionBySensor.
 */
ReadingPolar TrackerErrorModel::sampleOnce(double azimuth, double zenith, double distance) const{

    const double lambda = normal(this->terms.lambdaMm) / 1000.0;
    const double mu = normal(this->terms.mu);
    const double ex = normal(this->terms.exMm) / 1000.0;
    const double by = normal(this->terms.byMm) / 1000.0;
    const double bz = normal(this->terms.bzMm) / 1000.0;
    const double alpha = normal(this->terms.alphaArcsec) * arcsecToRad;
    const double gamma = normal(this->terms.gammaArcsec) * arcsecToRad;
    const double Aa1 = normal(this->terms.Aa1Arcsec) * arcsecToRad;
    const double Ba1 = normal(this->terms.Ba1Arcsec) * arcsecToRad;
    const double Aa2 = normal(this->terms.Aa2Arcsec) * arcsecToRad;
    const double Ba2 = normal(this->terms.Ba2Arcsec) * arcsecToRad;
    const double Ae0 = normal(this->terms.Ae0Arcsec) * arcsecToRad;
    const double Ae1 = normal(this->terms.Ae1Arcsec) * arcsecToRad;
    const double Be1 = normal(this->terms.Be1Arcsec) * arcsecToRad;
    const double Ae2 = normal(this->terms.Ae2Arcsec) * arcsecToRad;
    const double Be2 = normal(this->terms.Be2Arcsec) * arcsecToRad;

    double az = azimuth;
    double ze = zenith;
    double d = distance;

    d = (1 + mu) * d + lambda;

    const double azF1 = Aa1 * std::cos(az) + Ba1 * std::sin(az);
    const double azF2 = Aa2 * std::cos(2 * az) + Ba2 * std::sin(2 * az);
    az = az + azF1 + azF2;

    const double zeF1 = Ae1 * std::cos(ze) + Be1 * std::sin(ze);
    const double zeF2 = Ae2 * std::cos(2 * ze) + Be2 * std::sin(2 * ze);
    ze = ze + Ae0 + zeF1 + zeF2;

    OiVec ebb;
    ebb.add(-ex);
    ebb.add(by);
    ebb.add(bz);

    OiVec e00;
    e00.add(ex);
    e00.add(0.0);
    e00.add(0.0);

    OiVec xAxis(3);
    xAxis.setAt(0, 1.0);
    OiVec yAxis(3);
    yAxis.setAt(1, 1.0);
    OiVec zAxis(3);
    zAxis.setAt(2, 1.0);

    const OiMat Rz_Azimuth = OiMat::getRotationMatrix(az, zAxis);
    const OiMat Rx_alpha = OiMat::getRotationMatrix(alpha, xAxis);
    const OiMat Ry_zenith = OiMat::getRotationMatrix(ze - (M_PI / 2.0), yAxis);
    const OiMat Rx_minusAlpha = OiMat::getRotationMatrix(-1.0 * alpha, xAxis);
    const OiMat Rz_gamma = OiMat::getRotationMatrix(gamma, zAxis);

    const OiVec b = Rz_Azimuth * e00 + Rz_Azimuth * Rx_alpha * Ry_zenith * Rx_minusAlpha * ebb;
    const OiVec n = Rz_Azimuth * Rx_alpha * Ry_zenith * Rx_minusAlpha * Rz_gamma * xAxis;
    const OiVec p = b + d * n;

    ReadingPolar result;
    result.azimuth = std::atan2(p.getAt(1), p.getAt(0));
    result.distance = std::sqrt(p.getAt(0) * p.getAt(0) + p.getAt(1) * p.getAt(1) + p.getAt(2) * p.getAt(2));
    result.zenith = std::acos(p.getAt(2) / result.distance);
    result.isValid = true;

    return result;

}

/*!
 * \brief TrackerErrorModel::apply
 * Draws the actual returned reading once, then resamples the same model to
 * estimate the sigma it just applied - see the class comment for why this
 * is a resampling estimate rather than a closed-form propagation.
 */
ReadingPolar TrackerErrorModel::apply(double trueAzimuth, double trueZenith, double trueDistance) const{

    const ReadingPolar actual = this->sampleOnce(trueAzimuth, trueZenith, trueDistance);

    std::vector<double> az, ze, d;
    az.reserve(this->samples);
    ze.reserve(this->samples);
    d.reserve(this->samples);
    for(int i = 0; i < this->samples; i++){
        const ReadingPolar sample = this->sampleOnce(trueAzimuth, trueZenith, trueDistance);
        az.push_back(sample.azimuth);
        ze.push_back(sample.zenith);
        d.push_back(sample.distance);
    }

    ReadingPolar result = actual;
    result.sigmaAzimuth = stddev(az);
    result.sigmaZenith = stddev(ze);
    result.sigmaDistance = stddev(d);

    return result;

}
