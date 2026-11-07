#ifndef P_PSEUDOTRACKER_H
#define P_PSEUDOTRACKER_H
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QString>
#include <QThread>
#include <QtGlobal>
#include <atomic>
#include <cmath>
#include <map>
#include <vector>

#include "lasertracker.h"
#include "oimat.h"
#include "trackererrormodel.h"

using namespace oi;

/*!
 * \brief The PseudoTracker class
 */
class PseudoTracker : public LaserTracker {
 public:
  // ############################
  // sensor initialization method
  // ############################

  void init() override;

  // ########################
  // sensor state and actions
  // ########################

  bool doSelfDefinedAction(const QString& action) override;
  bool abortAction() override;

  //! connect app with laser tracker
  bool connectSensor() override;

  //! disconnect app with laser tracker
  bool disconnectSensor() override;

  //! laser tracker measures a point and returns an observation
  std::vector<oi::Guarded<Reading> > measure(
      const MeasurementConfig& mConfig) override;

  //! stream
  std::map<QString, QVariant> readingStream(
      const ReadingTypes& streamFormat) override;

  //! getConnectionState
  bool getConnectionState() override;

  //! return ready state of the sensor
  bool getIsReadyForMeasurement() override;

  //! sensor stats
  std::map<QString, QString> getSensorStatus() override;

  //! checks if sensor is busy
  bool getIsBusy() override;

  bool search() override;

 protected:
  //! starts initialization
  bool initialize() override;

  //! move laser tracker to specified position
  bool move(const double& azimuth, const double& zenith, const double& distance,
            const bool& isRelative) override;

  bool move(const double& x, const double& y, const double& z) override;

  //! sets laser tracke to home position
  bool home() override;

  //! turns motors on or off
  bool changeMotorState() override;

  //! toggle between frontside and backside
  bool toggleSightOrientation() override;

  //! compensation
  bool compensation() override;

 private:
  std::vector<oi::Guarded<Reading> > measurePolar(
      const MeasurementConfig& mConfig);
  std::vector<oi::Guarded<Reading> > measureDistance(
      const MeasurementConfig& mConfig);
  std::vector<oi::Guarded<Reading> > measureDirection(
      const MeasurementConfig& mConfig);
  std::vector<oi::Guarded<Reading> > measureCartesian(
      const MeasurementConfig& mConfig);
  std::vector<oi::Guarded<Reading> > measureLevel(
      const MeasurementConfig& mConfig);

  //! the true (noise-free) aim as a noisy polar draw, using this
  //! instance's configured error terms - sigmaAzimuth/sigmaZenith/
  //! sigmaDistance on the result are this model's own estimate of the
  //! noise it just applied, not a static default
  [[nodiscard]] ReadingPolar simulateAim() const;

  // ################
  // helper variables
  // ################

  double myAzimuth{};
  double myZenith{};
  double myDistance{};

  bool myMotor{};
  bool myInit{};
  bool myCompIt{};
  int side{};
  bool isConnected{};

  std::atomic<bool> isScanning;

  std::atomic<bool> returnReading;

  int measureTime{};  // [ms]
};

#endif  // P_PSEUDOTRACKER_H
