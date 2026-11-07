#ifndef SIMPLEPOLARMEASUREMENT_H
#define SIMPLEPOLARMEASUREMENT_H

#include <random>

#include "simulationmodel.h"
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
  bool analyseSimulationData(UncertaintyData& d) override;
  double getCorrelationCoefficient(const QList<double>& x,
                                   const QList<double>& y) override;

 private:
  // all supported distributions;
  QStringList distributions;

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

#endif  // SIMPLEPOLARMEASUREMENT_H
