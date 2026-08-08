#ifndef P_PSEUDOTRACKER_H
#define P_PSEUDOTRACKER_H

#include <atomic>
#include <QtGlobal>
#include <QDateTime>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QMap>
#include <QString>
#include <cmath>

#include "lasertracker.h"
#include "oimat.h"
#include "trackererrormodel.h"

using namespace oi;

/*!
 * \brief The PseudoTracker class
 */
class PseudoTracker : public LaserTracker
{
    Q_OBJECT

public:

    //############################
    //sensor initialization method
    //############################

    void init();

    //########################
    //sensor state and actions
    //########################

    bool doSelfDefinedAction(const QString &action);
    bool abortAction();

    //! connect app with laser tracker
    bool connectSensor();

    //! disconnect app with laser tracker
    bool disconnectSensor();

    //! laser tracker measures a point and returns an observation
    QList<QPointer<Reading> > measure(const MeasurementConfig &mConfig);

    //! stream
    QVariantMap readingStream(const ReadingTypes &streamFormat);

    //! getConnectionState
    bool getConnectionState();

    //! return ready state of the sensor
    bool getIsReadyForMeasurement();

    //!sensor stats
    QMap<QString, QString> getSensorStatus();

    //!checks if sensor is busy
    bool getIsBusy();

    bool search();

protected:

    //! starts initialization
    bool initialize();

    //! move laser tracker to specified position
    bool move(const double &azimuth, const double &zenith, const double &distance, const bool &isRelative);

    bool move(const double &x, const double &y, const double &z);

    //! sets laser tracke to home position
    bool home();

    //! turns motors on or off
    bool changeMotorState();

    //! toggle between frontside and backside
    bool toggleSightOrientation();

    //! compensation
    bool compensation();

private:
    QList<QPointer<Reading> > measurePolar(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureDistance(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureDirection(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureCartesian(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureLevel(const MeasurementConfig &mConfig);

    //! the true (noise-free) aim as a noisy polar draw, using this
    //! instance's configured error terms - sigmaAzimuth/sigmaZenith/
    //! sigmaDistance on the result are this model's own estimate of the
    //! noise it just applied, not a static default
    ReadingPolar simulateAim() const;

    //################
    //helper variables
    //################

    double myAzimuth;
    double myZenith;
    double myDistance;

    bool myMotor;
    bool myInit;
    bool myCompIt;
    int side;
    bool isConnected;

    std::atomic<bool> isScanning;

    std::atomic<bool> returnReading;

    int measureTime; // [ms]

};

#endif // P_PSEUDOTRACKER_H
