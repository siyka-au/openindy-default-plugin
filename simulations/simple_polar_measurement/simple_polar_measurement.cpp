#include "simple_polar_measurement.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "containers.h"

namespace {
// Edlen refractive-index formula (updated Edlen equation) coefficients
constexpr double kEdlenA1 = 13.928169;  // used negated
constexpr double kEdlenA2 = 34.7078238;
constexpr double kKelvinOffset = 273.15;
constexpr double kWaterTriplePoint = 273.16;  // [K]
constexpr int kEdlenExponent1 = 15;
constexpr double kEdlenExponent2 = 1.25;
constexpr double kSaturationPressureAtTriplePoint = 611.657;  // [pascal]
constexpr double kPercentPerUnit = 100.0;
constexpr double kEdlenA = 8342.54;
constexpr double kEdlenB = 2406147;
constexpr double kEdlenC = 15998;
constexpr double kEdlenD = 96095.43;
constexpr double kEdlenE = 0.601;
constexpr double kEdlenF = 0.00972;
constexpr double kEdlenG = 0.003661;
constexpr double kEdlenB2 = 130;
constexpr double kEdlenC2 = 38.9;
constexpr int kEdlenScale = 8;       // 10^-8 scaling of the group index terms
constexpr int kEdlenTermPower = 10;  // 10^-10 scaling of the humidity term
constexpr double kEdlenT0 = 292.75;
constexpr double kEdlenH0 = 3.7345;
constexpr double kEdlenH1 = 0.0401;
constexpr double kSensorLambdaSigma = 0.000403;
constexpr double kSensorMuSigma = 0.000005;
constexpr double kSensorExSigma = 0.0000122;
constexpr double kSensorBySigma = 0.0000654;
constexpr double kSensorBzSigma = 0.0000974;
constexpr double kSensorAlphaSigma = 0.128;
constexpr double kSensorGammaSigma = 0.079;
constexpr double kSensorAa1Sigma = 0.064;
constexpr double kSensorBa1Sigma = 0.080;
constexpr double kSensorAa2Sigma = 0.073;
constexpr double kSensorBa2Sigma = 0.090;
constexpr double kSensorAe0Sigma = 0.223;
constexpr double kSensorAe1Sigma = 0.152;
constexpr double kSensorBe1Sigma = 0.183;
constexpr double kSensorAe2Sigma = 0.214;
constexpr double kSensorBe2Sigma = 0.214;
constexpr double kThermalExpansionValue = 11.8;   // [1/K]
constexpr double kMaterialTemperatureValue = 20;  // [celsius]
constexpr double kTemperatureValue = 20.0;        // [celsius]
constexpr double kTemperatureUncertainty = 0.5;
constexpr double kVerticalTemperatureGradientValue = 0.4;
constexpr double kVerticalTemperatureGradientUncertainty = 0.1;
constexpr double kHorizontalTemperatureGradientValue = 0.1;
constexpr double kHorizontalTemperatureGradientUncertainty = 0.1;
constexpr double kPressureValue = 101325.0;  // [pascal]
constexpr double kPressureUncertainty = 10.0;
constexpr double kVerticalPressureGradientValue = -0.8;
constexpr double kVerticalPressureGradientUncertainty = 0.1;
constexpr double kHumidityValue = 50.0;  // [percent]
constexpr double kHumanDeltaAzimuthUncertainty = 0.0001;
constexpr double kHumanDeltaZenithUncertainty = 0.0001;
constexpr double kHumanDeltaDistanceUncertainty = 0.0001;
constexpr double kDefaultWavelength = 0.633;  // [micrometer], helium-neon laser
constexpr double kErfA1 = 0.254829592;        // Abramowitz-Stegun 7.1.26
constexpr double kErfA2 = -0.284496736;       // Abramowitz-Stegun 7.1.26
constexpr double kErfA3 = 1.421413741;        // Abramowitz-Stegun 7.1.26
constexpr double kErfA4 = -1.453152027;       // Abramowitz-Stegun 7.1.26
constexpr double kErfA5 = 1.061405429;        // Abramowitz-Stegun 7.1.26
constexpr double kErfP = 0.3275911;           // Abramowitz-Stegun 7.1.26
constexpr double kHalf = 0.5;
constexpr double kTwo = 2.0;
constexpr double kMicrometresPerMillimetre =
    1000.0;  // unit conversion for the sensor sigmas [um -> mm]
constexpr double kArcsecPerHalfTurn = 648000.0;  // 180 * 3600
constexpr double kMilliPerUnit = 1000.0;
constexpr double kDegreesPerHalfTurn = 180.0;
constexpr double kPartsPerMillion = 1000000.0;
constexpr double kChi2Limit95 =
    16.919;  // chi-square quantile, 95 percent, 9 degrees of freedom
constexpr double kHistogramClasses = 10.0;
}  // namespace

/*!
 * \brief SimplePolarMeasurement::init
 */
void SimplePolarMeasurement::init() {
  // set plugin meta data
  this->metaData.name = "SimplePolarMeasurement";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "mlux";
  this->metaData.description =
      "simulation of a simple polar measurement system";
  this->metaData.iid = Simulation_iidd;

  // simulation specific inits
  this->distributions.push_back("normal");
  this->distributions.push_back("uniform");
  this->distributions.push_back("triangular");
  newIteration = true;

  // ####################
  // sensor uncertainties
  // ####################

  // ######################### lambda #########################
  UncertaintyComponent lambda;

  lambda.name = "lambda";
  lambda.value = 0.0;
  lambda.uncertainty = kSensorLambdaSigma;
  lambda.distribution = this->distributions.at(0);
  lambda.distributions = this->distributions;
  lambda.errorUnit = "[mm]";

  lambda.description = "lambda - Range offset in millimeter";

  this->sensorUncertainties.insert_or_assign("lambda", lambda);

  // ######################### mu #########################
  UncertaintyComponent mu;

  mu.name = "mu";
  mu.value = 0.0;
  mu.uncertainty = kSensorMuSigma;
  mu.distribution = this->distributions.at(0);
  mu.distributions = this->distributions;
  mu.errorUnit = "[-]";

  mu.description = "mu - Scale factor for range";

  this->sensorUncertainties.insert_or_assign("mu", mu);

  // ######################### ex #########################
  UncertaintyComponent ex;

  ex.name = "ex";
  ex.value = 0.0;
  ex.uncertainty = kSensorExSigma;
  ex.distribution = this->distributions.at(0);
  ex.distributions = this->distributions;
  ex.errorUnit = "[mm]";

  ex.description = "ex - Transit axis offset from the standing axis";

  this->sensorUncertainties.insert_or_assign("ex", ex);

  // ######################### by #########################
  UncertaintyComponent by;

  by.name = "by";
  by.value = 0.0;
  by.uncertainty = kSensorBySigma;
  by.distribution = this->distributions.at(0);
  by.distributions = this->distributions;
  by.errorUnit = "[mm]";

  by.description = "by - Beam offset (y-direction) from the origin";

  this->sensorUncertainties.insert_or_assign("by", by);

  // ######################### bz #########################
  UncertaintyComponent bz;

  bz.name = "bz";
  bz.value = 0.0;
  bz.uncertainty = kSensorBzSigma;
  bz.distribution = this->distributions.at(0);
  bz.distributions = this->distributions;
  bz.errorUnit = "[mm]";

  bz.description = "bz - Beam offset (z-direction) from the origin";

  this->sensorUncertainties.insert_or_assign("bz", bz);

  // ######################### alpha #########################
  UncertaintyComponent alpha;

  alpha.name = "alpha";
  alpha.value = 0.0;
  alpha.uncertainty = kSensorAlphaSigma;
  alpha.distribution = this->distributions.at(0);
  alpha.distributions = this->distributions;
  alpha.errorUnit = "[arcsec]";

  alpha.description = "alpha - Transit axis angle in the yz-plane";

  this->sensorUncertainties.insert_or_assign("alpha", alpha);

  // ######################### gamma #########################
  UncertaintyComponent gamma;

  gamma.name = "gamma";
  gamma.value = 0.0;
  gamma.uncertainty = kSensorGammaSigma;
  gamma.distribution = this->distributions.at(0);
  gamma.distributions = this->distributions;
  gamma.errorUnit = "[arcsec]";

  gamma.description = "gamma - Beam axis angle in the xy-plane";

  this->sensorUncertainties.insert_or_assign("gamma", gamma);

  // ######################### Aa1 #########################
  UncertaintyComponent Aa1;

  Aa1.name = "Aa1";
  Aa1.value = 0.0;
  Aa1.uncertainty = kSensorAa1Sigma;
  Aa1.distribution = this->distributions.at(0);
  Aa1.distributions = this->distributions;
  Aa1.errorUnit = "[arcsec]";

  Aa1.description = "Aa1 - Azimuth scale error, first order";

  this->sensorUncertainties.insert_or_assign("Aa1", Aa1);

  // ######################### Ba1 #########################
  UncertaintyComponent Ba1;

  Ba1.name = "Aa1";
  Ba1.value = 0.0;
  Ba1.uncertainty = kSensorBa1Sigma;
  Ba1.distribution = this->distributions.at(0);
  Ba1.distributions = this->distributions;
  Ba1.errorUnit = "[arcsec]";

  Ba1.description = "Ba1 - Azimuth scale error, first order";

  this->sensorUncertainties.insert_or_assign("Ba1", Ba1);

  // ######################### Aa2 #########################
  UncertaintyComponent Aa2;

  Aa2.name = "Aa2";
  Aa2.value = 0.0;
  Aa2.uncertainty = kSensorAa2Sigma;
  Aa2.distribution = this->distributions.at(0);
  Aa2.distributions = this->distributions;
  Aa2.errorUnit = "[arcsec]";

  Aa2.description = "Aa2 - Azimuth scale error, second order";

  this->sensorUncertainties.insert_or_assign("Aa2", Aa2);

  // ######################### Ba2 #########################
  UncertaintyComponent Ba2;

  Ba2.name = "Ba2";
  Ba2.value = 0.0;
  Ba2.uncertainty = kSensorBa2Sigma;
  Ba2.distribution = this->distributions.at(0);
  Ba2.distributions = this->distributions;
  Ba2.errorUnit = "[arcsec]";

  Ba2.description = "Ba2 - Azimuth scale error, second order";

  this->sensorUncertainties.insert_or_assign("Ba2", Ba2);

  // ######################### Ba2 #########################
  UncertaintyComponent Ae0;

  Ae0.name = "Ae0";
  Ae0.value = 0.0;
  Ae0.uncertainty = kSensorAe0Sigma;
  Ae0.distribution = this->distributions.at(0);
  Ae0.distributions = this->distributions;
  Ae0.errorUnit = "[arcsec]";

  Ae0.description = "Ae0 - Elevation angle offset";

  this->sensorUncertainties.insert_or_assign("Ae0", Ae0);

  // ######################### Ae1 #########################
  UncertaintyComponent Ae1;

  Ae1.name = "Ae1";
  Ae1.value = 0.0;
  Ae1.uncertainty = kSensorAe1Sigma;
  Ae1.distribution = this->distributions.at(0);
  Ae1.distributions = this->distributions;
  Ae1.errorUnit = "[arcsec]";

  Ae1.description = "Ae1 - Elevation scale error, first order";

  this->sensorUncertainties.insert_or_assign("Ae1", Ae1);

  // ######################### Be1 #########################
  UncertaintyComponent Be1;

  Be1.name = "Be1";
  Be1.value = 0.0;
  Be1.uncertainty = kSensorBe1Sigma;
  Be1.distribution = this->distributions.at(0);
  Be1.distributions = this->distributions;
  Be1.errorUnit = "[arcsec]";

  Be1.description = "Be1 - Elevation scale error, first order";

  this->sensorUncertainties.insert_or_assign("Be1", Be1);

  // ######################### Ae2 #########################
  UncertaintyComponent Ae2;

  Ae2.name = "Ae2";
  Ae2.value = 0.0;
  Ae2.uncertainty = kSensorAe2Sigma;
  Ae2.distribution = this->distributions.at(0);
  Ae2.distributions = this->distributions;
  Ae2.errorUnit = "[arcsec]";

  Ae2.description = "Ae2 - Elevation scale error, second order";

  this->sensorUncertainties.insert_or_assign("Ae2", Ae2);

  // ######################### Be2 #########################
  UncertaintyComponent Be2;

  Be2.name = "Be2";
  Be2.value = 0.0;
  Be2.uncertainty = kSensorBe2Sigma;
  Be2.distribution = this->distributions.at(0);
  Be2.distributions = this->distributions;
  Be2.errorUnit = "[arcsec]";

  Be2.description = "Be2 - Elevation scale error, second order";

  this->sensorUncertainties.insert_or_assign("Be2", Be2);

  // ####################
  // object uncertainties
  // ####################

  // ######################### coefficient of thermal expansion
  // #########################
  UncertaintyComponent cOfE;

  cOfE.name = "coefficient of thermal expansion";
  cOfE.value = kThermalExpansionValue;
  cOfE.uncertainty = 2;
  cOfE.distribution = this->distributions.at(0);
  cOfE.distributions = this->distributions;
  cOfE.errorUnit = "1/K";

  cOfE.description =
      "The degree of expansion divided by the change in temperaturer";

  this->objectUncertainties.insert_or_assign("coefficientOfExpansion", cOfE);

  // ######################### materialTemperature #########################
  UncertaintyComponent mt;

  mt.name = "material temperature";
  mt.value = kMaterialTemperatureValue;
  mt.uncertainty = 1;
  mt.distribution = this->distributions.at(0);
  mt.distributions = this->distributions;
  mt.errorUnit = "[celsius]";

  mt.description = "influence of temperaturer on the object and sensor";

  this->objectUncertainties.insert_or_assign("materialTemperature", mt);

  // #########################
  // environment uncertainites
  // #########################

  // ######################### temperature #########################
  UncertaintyComponent t;

  t.name = "temperature";
  t.value = kTemperatureValue;
  t.uncertainty = kTemperatureUncertainty;
  t.distribution = this->distributions.at(0);
  t.distributions = this->distributions;
  t.errorUnit = "[celsius]";

  t.description = "influence of temperaturer on the object and sensor";

  this->environmentUncertainties.insert_or_assign("temperature", t);

  // ######################### vertical temperature
  // gradient#########################
  UncertaintyComponent vtg;

  vtg.name = "vertical temperature gradient";
  vtg.value = kVerticalTemperatureGradientValue;
  vtg.uncertainty = kVerticalTemperatureGradientUncertainty;
  vtg.distribution = this->distributions.at(0);
  vtg.distributions = this->distributions;
  vtg.errorUnit = "[celsius/m]";

  vtg.description = "gradient to calculate the vertical beam refraction";

  this->environmentUncertainties.insert_or_assign("verticalTemperatureGradient",
                                                  vtg);

  // ######################### horizontal temperature
  // gradient#########################
  UncertaintyComponent htg;

  htg.name = "horizontal temperature gradient";
  htg.value = kHorizontalTemperatureGradientValue;
  htg.uncertainty = kHorizontalTemperatureGradientUncertainty;
  htg.distribution = this->distributions.at(0);
  htg.distributions = this->distributions;
  htg.errorUnit = "[celsius/m]";

  htg.description = "gradient to calculate the horizontal beam refraction";

  this->environmentUncertainties.insert_or_assign(
      "horizontalTemperatureGradient", htg);

  // ######################### pressure #########################
  UncertaintyComponent p;

  p.name = "pressure";
  p.value = kPressureValue;
  p.uncertainty = kPressureUncertainty;
  p.distribution = this->distributions.at(0);
  p.distributions = this->distributions;
  p.errorUnit = "[pascal]";

  p.description = "influence of pressure(air)  on the sensor";

  this->environmentUncertainties.insert_or_assign("pressure", p);

  // ######################### vertical pressure
  // Gradient#########################
  UncertaintyComponent hpg;

  hpg.name = "vertical pressure gradient";
  hpg.value = kVerticalPressureGradientValue;
  hpg.uncertainty = kVerticalPressureGradientUncertainty;
  hpg.distribution = this->distributions.at(0);
  hpg.distributions = this->distributions;
  hpg.errorUnit = "[pascal/m]";

  hpg.description = "gradient to calculate the vertical beam refraction";

  this->environmentUncertainties.insert_or_assign("verticalPressureGradient",
                                                  hpg);

  // ######################### humidity #########################
  UncertaintyComponent h;

  h.name = "humidity";
  h.value = kHumidityValue;
  h.uncertainty = 1.0;
  h.distribution = this->distributions.at(0);
  h.distributions = this->distributions;
  h.errorUnit = "[percent %]";

  h.description = "influence of humidity(air)  on the sensor";

  this->environmentUncertainties.insert_or_assign("humidity", h);

  // ###############
  // human influence
  // ###############

  // ######################### azimuth #########################
  UncertaintyComponent a;

  a.name = "delta_azimuth";
  a.value = 0.0;
  a.uncertainty = kHumanDeltaAzimuthUncertainty;
  a.distribution = this->distributions.at(0);
  a.distributions = this->distributions;
  a.errorUnit = "[milliGrad]";

  a.description = "influence of the user on the azimuth measurement";

  this->humanInfluence.insert_or_assign("delta_azimuth", a);

  // ######################### zenith #########################
  UncertaintyComponent z;

  z.name = "delta_zenith";
  z.value = 0.0;
  z.uncertainty = kHumanDeltaZenithUncertainty;
  z.distribution = this->distributions.at(0);
  z.distributions = this->distributions;
  z.errorUnit = "[milliGrad]";

  z.description = "influence of the user on the zenith measurement";

  this->humanInfluence.insert_or_assign("delta_zenith", z);

  // ######################### distance #########################
  UncertaintyComponent d;

  d.name = "delta_distance";
  d.value = 0.0;
  d.uncertainty = kHumanDeltaDistanceUncertainty;
  d.distribution = this->distributions.at(0);
  d.distributions = this->distributions;
  d.errorUnit = "[mm]";

  d.description = "influence of the user on the distance measurement";

  this->humanInfluence.insert_or_assign("delta_distance", d);

  // set double parameters
  this->doubleParameters.insert_or_assign("wavelength [micrometer]",
                                          kDefaultWavelength);

  // set string parameters
  this->stringParameters.emplace("use sensor errors", "no");
  this->stringParameters.emplace("use sensor errors", "yes");
  this->stringParameters.emplace("use environment errors", "no");
  this->stringParameters.emplace("use environment errors", "yes");
  this->stringParameters.emplace("use object errors", "yes");
  this->stringParameters.emplace("use object errors", "no");
  this->stringParameters.emplace("use human errors", "yes");
  this->stringParameters.emplace("use human errors", "no");
}

double erfFunc(double x) {
  // constants
  double a1 = kErfA1;
  double a2 = kErfA2;
  double a3 = kErfA3;
  double a4 = kErfA4;
  double a5 = kErfA5;
  double p = kErfP;

  // Save the sign of x
  int sign = 1;
  if (x < 0) sign = -1;
  x = fabs(x);

  // A&S formula 7.1.26
  double t = 1.0 / (1.0 + p * x);
  double y =
      1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);

  return sign * y;
}

double densityNormal(const double& x, const double& expectation,
                     const double& uncertainty, const double& lowerLimit,
                     const double& upperLimit) {
  double e = -kHalf * (((x - expectation) / (uncertainty)) *
                       ((x - expectation) / (uncertainty)));

  double result = (1.0 / (uncertainty * sqrt(kTwo * M_PI))) * std::exp(e);

  return result;
}

double distributionNormal(const double& x, const double& expectation,
                          const double& uncertainty, const double& lowerLimit,
                          const double& upperLimit) {
  return kHalf * (1.0 + erfFunc((x - expectation) /
                                (sqrt(kTwo * uncertainty * uncertainty))));
}

double densityUniform(const double& x, const double& expectation,
                      const double& uncertainty, const double& lowerLimit,
                      const double& upperLimit) {
  double result = 1 / (upperLimit - lowerLimit);

  return result;
}

double distributionUniform(const double& x, const double& expectation,
                           const double& uncertainty, const double& lowerLimit,
                           const double& upperLimit) {
  double result = (x - lowerLimit) / (upperLimit - lowerLimit);

  return result;
}

double densityTriangular(const double& x, const double& expectation,
                         const double& uncertainty, const double& lowerLimit,
                         const double& upperLimit) {
  double result = 0.0;

  if (lowerLimit <= x && x <= expectation) {
    result = (2 * (x - lowerLimit)) /
             ((upperLimit - lowerLimit) * (expectation - lowerLimit));
  } else if (expectation < x && x <= upperLimit) {
    result = (2 * (upperLimit - x)) /
             ((upperLimit - lowerLimit) * (upperLimit - expectation));
  }

  return result;
}

double distributionTriangular(const double& x, const double& expectation,
                              const double& uncertainty,
                              const double& lowerLimit,
                              const double& upperLimit) {
  double result = 0.0;

  if (lowerLimit <= x && x <= expectation) {
    result = ((x - lowerLimit) * (x - lowerLimit)) /
             ((upperLimit - lowerLimit) * (expectation - lowerLimit));
  } else if (expectation < x && x <= upperLimit) {
    result = 1.0 - (((upperLimit - x) * (upperLimit - x)) /
                    ((upperLimit - lowerLimit) * (upperLimit - expectation)));
  }

  return result;
}

/*!
 * \brief SimplePolarMeasurement::analyseSimulationData
 * analyse the simulation values saved in d.values. store your results
 * \param d
 * \return
 */
bool SimplePolarMeasurement::analyseSimulationData(
    UncertaintyData& d, const OperationContext& /*ctx*/) {
  if (d.values.size() != 0) {
    double sumDD = 0.0;

    d.maxValue = d.values.at(0);
    d.minValue = d.values.at(0);

    for (int i = 0; std::cmp_less(i, d.values.size()); i++) {
      if (d.values.at(i) > d.maxValue) {
        d.maxValue = d.values.at(i);
      }
      if (d.values.at(i) < d.minValue) {
        d.minValue = d.values.at(i);
      }
      sumDD += d.values.at(i);
    }

    d.expectation = sumDD / static_cast<double>(d.values.size());

    this->calcUncertainty(d);
    this->checkDistribution(d);

    if (d.distribution.compare("normal") == 0) {
      d.densityFunction = densityNormal;
      d.distributionFunction = distributionNormal;
    } else if (d.distribution.compare("uniform") == 0) {
      d.densityFunction = densityUniform;
      d.distributionFunction = distributionUniform;
    } else if (d.distribution.compare("triangular") == 0) {
      d.densityFunction = densityTriangular;
      d.distributionFunction = distributionTriangular;
    }

  } else {
    return false;
  }

  return true;
}

/*!
 * \brief SimplePolarMeasurement::getCorrelationCoefficient
 * determine the correlation coefficient of the two quantities x and y.
 * \param x
 * \param y
 * \return
 */
double SimplePolarMeasurement::getCorrelationCoefficient(
    const std::vector<double>& x, const std::vector<double>& y) {
  if (x.size() != y.size()) {
    return 0.0;
  }

  double sumXX = 0.0;
  double sumYY = 0.0;

  for (int i = 0; std::cmp_less(i, x.size()); i++) {
    sumXX += x.at(i);
    sumYY += y.at(i);
  }

  double mX = sumXX / static_cast<double>(x.size());
  double mY = sumYY / static_cast<double>(y.size());

  double sumXi2 = 0.0;
  double sumYi2 = 0.0;
  double sumXY = 0.0;

  for (int i = 0; std::cmp_less(i, x.size()); i++) {
    sumXi2 += (x.at(i) - mX) * (x.at(i) - mX);
    sumYi2 += (y.at(i) - mY) * (y.at(i) - mY);
    sumXY += (x.at(i) - mX) * (y.at(i) - mY);
  }

  double r = sumXY / (sqrt(sumXi2 * sumYi2));

  return r;
}

/*!
 * \brief SimplePolarMeasurement::distort
 * here you have to distort a reading with the given uncertainties.
 * objectRelation is a homogenous matrix (4x4) which describes the
 * relation between Station and Object
 * \param r
 * \param objectRelation
 * \param newIterationStart
 * \return
 */
bool SimplePolarMeasurement::distort(const oi::Guarded<Reading>& r,
                                     const OiMat& objectRelation,
                                     const bool& newIterationStart) {
  newIteration = newIterationStart;

  if (oi::value(this->sConfig.stringParameters, "use sensor errors")
          .compare("yes") == 0) {
    distortionBySensor(r);
  }

  if (oi::value(this->sConfig.stringParameters, "use environment errors")
          .compare("yes") == 0) {
    distortionByEnviroment(r);
  }

  if (oi::value(this->sConfig.stringParameters, "use object errors")
          .compare("yes") == 0) {
    distortionByObject(r, objectRelation);
  }

  if (oi::value(this->sConfig.stringParameters, "use human errors")
          .compare("yes") == 0) {
    distortionByHuman(r);
  }

  return true;
}

double SimplePolarMeasurement::distortComponent(UncertaintyComponent u) {
  if (u.uncertainty == 0.0) {
    return u.value;
  }

  if (u.distribution.compare("normal") == 0) {
    std::normal_distribution<double> dist_Normal(u.value, u.uncertainty);
    std::random_device rd;
    std::default_random_engine generator(rd());

    return dist_Normal(generator);

  } else if (u.distribution.compare("uniform") == 0) {
    double a = u.value - u.uncertainty;
    double b = u.value + u.uncertainty;

    std::uniform_real_distribution<double> dist_Uniform(a, b);
    std::random_device rd;
    std::default_random_engine generator(rd());

    return dist_Uniform(generator);

  } else if (u.distribution.compare("triangular") == 0) {
    double a = u.value - u.uncertainty;
    double b = u.value + u.uncertainty;
    double c = u.value;

    std::uniform_real_distribution<double> dist_Uniform(0, 1);
    std::random_device rd;
    std::default_random_engine generator(rd());

    double U = dist_Uniform(generator);
    double F = (c - a) / (b - a);

    if (U <= F)
      return a + sqrt(U * (b - a) * (c - a));
    else
      return b - sqrt((1 - U) * (b - a) * (b - c));
  }

  return u.value;
}

bool SimplePolarMeasurement::distortionBySensor(Reading* r) {
  if (r->getTypeOfReading() != ReadingTypes::ePolarReading) {
    return false;
  }

  double lambda =
      distortComponent(oi::value(
          this->sConfig.uncertainties.sensorUncertainties, "lambda")) /
      kMicrometresPerMillimetre;
  double mu = distortComponent(
      oi::value(this->sConfig.uncertainties.sensorUncertainties, "mu"));
  double ex = distortComponent(oi::value(
                  this->sConfig.uncertainties.sensorUncertainties, "ex")) /
              kMicrometresPerMillimetre;
  double by = distortComponent(oi::value(
                  this->sConfig.uncertainties.sensorUncertainties, "by")) /
              kMicrometresPerMillimetre;
  double bz = distortComponent(oi::value(
                  this->sConfig.uncertainties.sensorUncertainties, "bz")) /
              kMicrometresPerMillimetre;
  double alpha =
      distortComponent(
          oi::value(this->sConfig.uncertainties.sensorUncertainties, "alpha")) *
      (M_PI / kArcsecPerHalfTurn);
  double gamma =
      distortComponent(
          oi::value(this->sConfig.uncertainties.sensorUncertainties, "gamma")) *
      (M_PI / kArcsecPerHalfTurn);
  double Aa1 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Aa1")) *
               (M_PI / kArcsecPerHalfTurn);
  double Ba1 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Ba1")) *
               (M_PI / kArcsecPerHalfTurn);
  double Aa2 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Aa2")) *
               (M_PI / kArcsecPerHalfTurn);
  double Ba2 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Ba2")) *
               (M_PI / kArcsecPerHalfTurn);
  double Ae0 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Ae0")) *
               (M_PI / kArcsecPerHalfTurn);
  double Ae1 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Ae1")) *
               (M_PI / kArcsecPerHalfTurn);
  double Be1 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Be1")) *
               (M_PI / kArcsecPerHalfTurn);
  double Ae2 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Ae2")) *
               (M_PI / kArcsecPerHalfTurn);
  double Be2 = distortComponent(oi::value(
                   this->sConfig.uncertainties.sensorUncertainties, "Be2")) *
               (M_PI / kArcsecPerHalfTurn);

  double az = r->getPolarReading().azimuth;
  double ze = r->getPolarReading().zenith;
  double d = r->getPolarReading().distance;

  d = (1 + mu) * d + lambda;

  double azF1 = Aa1 * cos(az) + Ba1 * sin(az);
  double azF2 = Aa2 * cos(2 * az) + Ba2 * sin(2 * az);

  az = az + azF1 + azF2;

  double zeF1 = Ae1 * cos(ze) + Be1 * sin(ze);
  double zeF2 = Ae2 * cos(2 * ze) + Be2 * sin(2 * ze);

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

  OiMat Rz_Azimuth = OiMat::getRotationMatrix(az, zAxis);
  OiMat Rx_alpha = OiMat::getRotationMatrix(alpha, xAxis);
  OiMat Ry_zenith = OiMat::getRotationMatrix(ze - (M_PI / kTwo), yAxis);
  OiMat Rx_minusAlpha = OiMat::getRotationMatrix(-1.0 * alpha, xAxis);
  OiMat Rz_gamma = OiMat::getRotationMatrix(gamma, zAxis);

  OiVec b(3);
  b = Rz_Azimuth * e00 +
      Rz_Azimuth * Rx_alpha * Ry_zenith * Rx_minusAlpha * ebb;
  ;

  OiVec n(3);
  n = Rz_Azimuth * Rx_alpha * Ry_zenith * Rx_minusAlpha * Rz_gamma * xAxis;

  OiVec p(3);
  p = b + d * n;

  ReadingPolar rPolar;

  rPolar.azimuth = qAtan2(p.getAt(1), p.getAt(0));
  rPolar.distance =
      std::sqrt(p.getAt(0) * p.getAt(0) + p.getAt(1) * p.getAt(1) +
                p.getAt(2) * p.getAt(2));
  rPolar.zenith = acos(p.getAt(2) / rPolar.distance);

  r->setPolarReading(rPolar);

  return true;
}

bool SimplePolarMeasurement::distortionByEnviroment(Reading* r) {
  if (newIteration) {
    double refTemperature =
        oi::value(this->sConfig.uncertainties.enviromentUncertainties,
                  "temperature")
            .value;
    double temperature = distortComponent(oi::value(
        this->sConfig.uncertainties.enviromentUncertainties, "temperature"));

    double verticalTempGradient = distortComponent(
        oi::value(this->sConfig.uncertainties.enviromentUncertainties,
                  "verticalTemperatureGradient"));
    double horizontalTempGradient = distortComponent(
        oi::value(this->sConfig.uncertainties.enviromentUncertainties,
                  "horizontalTemperatureGradient"));
    double verticalPreGradient = distortComponent(
        oi::value(this->sConfig.uncertainties.enviromentUncertainties,
                  "verticalPressureGradient"));

    double refPressure =
        oi::value(this->sConfig.uncertainties.enviromentUncertainties,
                  "pressure")
            .value;
    double pressure = distortComponent(oi::value(
        this->sConfig.uncertainties.enviromentUncertainties, "pressure"));

    double refHumidity =
        oi::value(this->sConfig.uncertainties.enviromentUncertainties,
                  "humidity")
            .value;
    double humidity = distortComponent(oi::value(
        this->sConfig.uncertainties.enviromentUncertainties, "humidity"));

    double wavelength =
        oi::value(this->sConfig.doubleParameters, "wavelength [micrometer]");

    refraction = this->edlenRefractionCalculation(refTemperature, refPressure,
                                                  refHumidity, wavelength);
    distortedRefraction = this->edlenRefractionCalculation(
        temperature, pressure, humidity, wavelength);

    verticalDn = this->edlenRefractionCalculation(
        refTemperature + verticalTempGradient,
        refPressure + verticalPreGradient, refHumidity, wavelength);
    verticalDn = verticalDn - refraction;

    horizontalDn = this->edlenRefractionCalculation(
        refTemperature + horizontalTempGradient, refPressure, refHumidity,
        wavelength);
    horizontalDn = horizontalDn - refraction;
  }

  ReadingPolar rPolar;

  rPolar.distance =
      r->getPolarReading().distance +
      ((refraction - distortedRefraction) * r->getPolarReading().distance);

  double refractionZenith =
      (1 / (2 * refraction)) * verticalDn * rPolar.distance;
  double refractionAzimuth =
      (1 / (2 * refraction)) * horizontalDn * rPolar.distance;

  rPolar.azimuth = r->getPolarReading().azimuth + refractionAzimuth;
  rPolar.zenith = r->getPolarReading().zenith + refractionZenith;

  r->setPolarReading(rPolar);

  return true;
}

bool SimplePolarMeasurement::distortionByHuman(Reading* r) {
  double deltaAzimuth =
      distortComponent(oi::value(this->sConfig.uncertainties.humanUncertainties,
                                 "delta_azimuth")) /
      kMilliPerUnit;
  deltaAzimuth = deltaAzimuth * M_PI / kDegreesPerHalfTurn;
  double deltaZenith =
      distortComponent(oi::value(this->sConfig.uncertainties.humanUncertainties,
                                 "delta_zenith")) /
      kMilliPerUnit;
  deltaZenith = deltaZenith * M_PI / kDegreesPerHalfTurn;
  double deltaDistance =
      distortComponent(oi::value(this->sConfig.uncertainties.humanUncertainties,
                                 "delta_distance")) /
      kMilliPerUnit;

  ReadingPolar rPolar;

  rPolar.azimuth = r->getPolarReading().azimuth + deltaAzimuth;
  rPolar.zenith = r->getPolarReading().zenith + deltaZenith;
  rPolar.distance = r->getPolarReading().distance + deltaDistance;

  r->setPolarReading(rPolar);

  return true;
}

bool SimplePolarMeasurement::distortionByObject(Reading* r,
                                                OiMat objectRelation) {
  if (objectRelation.getRowCount() != 4 && objectRelation.getColCount() != 4) {
    return false;
  }

  if (newIteration) {
    ref_coefficientOfExpansion =
        oi::value(this->sConfig.uncertainties.objectUncertainties,
                  "coefficientOfExpansion")
            .value;
    coefficientOfExpansion = distortComponent(
        oi::value(this->sConfig.uncertainties.objectUncertainties,
                  "coefficientOfExpansion"));

    ref_materialTemperature =
        oi::value(this->sConfig.uncertainties.objectUncertainties,
                  "materialTemperature")
            .value;
    materialTemperature = distortComponent(
        oi::value(this->sConfig.uncertainties.objectUncertainties,
                  "materialTemperature"));
  }

  double scale = 1 + ((materialTemperature - ref_materialTemperature) *
                      (coefficientOfExpansion - ref_coefficientOfExpansion) /
                      kPartsPerMillion);

  OiVec xyz = Reading::toCartesian(r->getPolarReading().azimuth,
                                   r->getPolarReading().zenith,
                                   r->getPolarReading().distance);
  xyz = objectRelation * xyz;
  xyz = xyz * scale;
  OiVec polar = Reading::toPolar(xyz.getAt(0), xyz.getAt(1), xyz.getAt(2));

  ReadingPolar rPolar;

  rPolar.azimuth = polar.getAt(0);
  rPolar.zenith = polar.getAt(1);
  rPolar.distance = polar.getAt(2);

  r->setPolarReading(rPolar);

  return true;
}

void SimplePolarMeasurement::checkDistribution(UncertaintyData& d) {
  double chi95 = kChi2Limit95;
  double chi2Normal = 0.0;
  double chi2Uniform = 0.0;

  std::vector<double> tmpList = d.values;
  double n = static_cast<double>(tmpList.size());
  std::sort(tmpList.begin(), tmpList.end());

  double classification = (d.maxValue - d.minValue) / kHistogramClasses;
  double startClass = tmpList.front();
  double endClass = tmpList.front() + classification;
  double hi = 0.0;

  for (int i = 0; i < n; i++) {
    if (tmpList.at(i) >= endClass || i == n - 1) {
      double startNormal = distributionNormal(
          startClass, d.expectation, d.uncertainty, d.minValue, d.maxValue);
      double endNormal = distributionNormal(
          endClass, d.expectation, d.uncertainty, d.minValue, d.maxValue);
      double startUniform = distributionUniform(
          startClass, d.expectation, d.uncertainty, d.minValue, d.maxValue);
      double endUniform = distributionUniform(
          endClass, d.expectation, d.uncertainty, d.minValue, d.maxValue);

      chi2Normal += ((hi - n * (endNormal - startNormal)) *
                     (hi - n * (endNormal - startNormal))) /
                    (n * (endNormal - startNormal));
      chi2Uniform += ((hi - n * (endUniform - startUniform)) *
                      (hi - n * (endUniform - startUniform))) /
                     (n * (endUniform - startUniform));

      startClass = tmpList.at(i);
      endClass = tmpList.at(i) + classification;
      hi = 1.0;

    } else {
      hi += 1.0;
    }
  }

  if (chi95 > chi2Normal) {
    d.distribution = "normal";
  } else if (chi95 > chi2Uniform) {
    d.distribution = "uniform";
  } else {
    d.distribution = "triangular";
  }
}

void SimplePolarMeasurement::calcUncertainty(UncertaintyData& d) {
  double sumVV = 0.0;

  for (double v : d.values) {
    sumVV += (d.expectation - v) * (d.expectation - v);
  }

  d.uncertainty = sqrt(sumVV / (static_cast<double>(d.values.size()) - 1.0));
}

double SimplePolarMeasurement::edlenRefractionCalculation(double temperature,
                                                          double pressure,
                                                          double humidity,
                                                          double wavelength) {
  double A1 = -kEdlenA1;
  double A2 = kEdlenA2;

  double T = temperature + kKelvinOffset;
  double Phi = T / kWaterTriplePoint;
  double Y = A1 * (1 - pow(Phi, -kEdlenExponent1)) +
             A2 * (1 - pow(Phi, -kEdlenExponent2));

  double Psv = kSaturationPressureAtTriplePoint * exp(Y);
  double pv = humidity / kPercentPerUnit * Psv;

  double A = kEdlenA;
  double B = kEdlenB;
  double C = kEdlenC;
  double D = kEdlenD;
  double E = kEdlenE;
  double F = kEdlenF;
  double G = kEdlenG;

  double S = 1 / (wavelength * wavelength);

  double Ns =
      1 + pow(10, -kEdlenScale) * (A + B / (kEdlenB2 - S) + C / (kEdlenC2 - S));
  double X = (1 + pow(10, -kEdlenScale) * (E - F * temperature)) /
             (1 + G * temperature);

  double Ntp = 1 + pressure * (Ns - 1.0) * X / D;

  return Ntp - pow(-10, -kEdlenTermPower) *
                   (kEdlenT0 / (temperature + kKelvinOffset)) *
                   (kEdlenH0 - kEdlenH1 * S) * pv;
}
