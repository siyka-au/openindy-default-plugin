#ifndef OI_DEFAULT_PLUGIN_SIMULATIONS_SIMPLE_POLAR_MEASUREMENT_SIMPLE_POLAR_MEASUREMENT_H_
#define OI_DEFAULT_PLUGIN_SIMULATIONS_SIMPLE_POLAR_MEASUREMENT_SIMPLE_POLAR_MEASUREMENT_H_
#include <random>
#include <vector>

#include "simulation_model.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <QDebug>
#include <QtAlgorithms>

using namespace oi;

/*!
 * \brief The SimplePolarMeasurement class
 *
 *  implementation of a simple simulation of a
 *  polar measurement system such as a Laser Tracker
 *  or Total Station. The Sensor model is described by
 *  Ben Hughes(Laser tracker error determination using
 *  a network measurement)
 */
class SimplePolarMeasurement : public SimulationModel {
 public:
  // ################################
  // simulation initialization method
  // ################################

  void init() override;

  // ##################
  // simulation methods
  // ##################

  bool distort(const oi::Guarded<Reading>& r, const OiMat& objectRelation,
               const bool& newIterationStart) override;
  bool analyseSimulationData(UncertaintyData& d,
                             const OperationContext& ctx = {}) override;
  double getCorrelationCoefficient(const std::vector<double>& x,
                                   const std::vector<double>& y) override;

 private:
  // all supported distributions;
  std::vector<std::string> distributions;

  double variance{};
  double expectation{};

  bool newIteration{};

  double refraction{};
  double distortedRefraction{};
  double verticalDn{};
  double horizontalDn{};

  double ref_coefficientOfExpansion{};
  double ref_materialTemperature{};
  double coefficientOfExpansion{};
  double materialTemperature{};

  double distortComponent(UncertaintyComponent u);

  bool distortionBySensor(Reading* r);
  bool distortionByEnviroment(Reading* r);
  bool distortionByHuman(Reading* r);
  bool distortionByObject(Reading* r, OiMat objectRelation);

  void checkDistribution(UncertaintyData& d);
  void calcUncertainty(UncertaintyData& d);

  double edlenRefractionCalculation(double temperature, double pressure,
                                    double humidity, double wavelength);
};

#endif  // OI_DEFAULT_PLUGIN_SIMULATIONS_SIMPLE_POLAR_MEASUREMENT_SIMPLE_POLAR_MEASUREMENT_H_
