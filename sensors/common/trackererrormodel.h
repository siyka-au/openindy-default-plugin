#ifndef TRACKERERRORMODEL_H
#define TRACKERERRORMODEL_H

#include "reading.h"

using namespace oi;

/*!
 * \brief The TrackerErrorTerms struct
 * The Ben Hughes polar-tracker error model (Hughes, Sun, Forbes, Lewis 2010,
 * "Determining laser tracker alignment errors using a network measurement",
 * CMSC Journal Autumn 2010, 26-32). Each term is the one-sigma width of a
 * zero-mean normally distributed random variable - the same shape
 * PseudoTracker and SimplePolarMeasurement both already used, just
 * previously duplicated verbatim between the two.
 *
 * Units match the original parameter names: distances in millimetres,
 * angles in arcseconds, mu dimensionless.
 */
struct TrackerErrorTerms{
    double lambdaMm = 0.0;   //range offset
    double mu = 0.0;         //range scale factor
    double exMm = 0.0;       //transit axis offset from the standing axis
    double byMm = 0.0;       //beam offset (y) from the origin
    double bzMm = 0.0;       //beam offset (z) from the origin
    double alphaArcsec = 0.0;
    double gammaArcsec = 0.0;
    double Aa1Arcsec = 0.0;
    double Ba1Arcsec = 0.0;
    double Aa2Arcsec = 0.0;
    double Ba2Arcsec = 0.0;
    double Ae0Arcsec = 0.0;
    double Ae1Arcsec = 0.0;
    double Be1Arcsec = 0.0;
    double Ae2Arcsec = 0.0;
    double Be2Arcsec = 0.0;
};

/*!
 * \brief The TrackerErrorModel class
 * Applies TrackerErrorTerms to a true polar observation and reports the
 * sigma it actually just applied, instead of a static configured default.
 *
 * The error terms propagate through several rotation matrices before
 * producing azimuth/zenith/distance, so there is no clean closed-form
 * sigma for the result. Rather than derive and maintain a linearised
 * Jacobian of that whole chain, apply() estimates sigma the same way a
 * real uncertainty analysis would validate one: by resampling the same
 * generative model many times and measuring the scatter it actually
 * produces. This is only viable because the model runs in microseconds
 * and nothing here talks to real hardware - a real driver would use a
 * closed-form accuracy spec from its own calibration data instead.
 */
class TrackerErrorModel{

public:
    explicit TrackerErrorModel(const TrackerErrorTerms &oneSigmaTerms, int sigmaEstimationSamples = 256);

    //! one noisy draw from a true polar observation, with sigmaAzimuth/
    //! sigmaZenith/sigmaDistance set from the model's own resampled scatter
    ReadingPolar apply(double trueAzimuth, double trueZenith, double trueDistance) const;

private:
    ReadingPolar sampleOnce(double azimuth, double zenith, double distance) const;

    TrackerErrorTerms terms;
    int samples;

};

#endif // TRACKERERRORMODEL_H
