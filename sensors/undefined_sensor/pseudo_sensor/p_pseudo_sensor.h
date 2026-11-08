#ifndef OI_DEFAULT_PLUGIN_SENSORS_UNDEFINED_SENSOR_PSEUDO_SENSOR_P_PSEUDO_SENSOR_H_
#define OI_DEFAULT_PLUGIN_SENSORS_UNDEFINED_SENSOR_PSEUDO_SENSOR_P_PSEUDO_SENSOR_H_

#include <QtCore>

#include "sensor.h"

class PseudoSensor : public Sensor {
 public:
  PseudoSensor();

  QList<Configuration::ReadingTypes>* getSupportedReadingTypes() const;
  QList<Configuration::SensorFunctionalities> getSupportedSensorActions() const;
  QList<Configuration::ConnectionTypes>* getConnectionType() const;

  PluginMetaData* getMetaData() const;

  QMap<std::string, int>* getIntegerParameter() const;
  QMap<std::string, double>* getDoubleParameter() const;
  QMap<std::string, std::vector<std::string>>* getStringParameter() const;
  std::vector<std::string> selfDefinedActions() const;
  bool doSelfDefinedAction(std::string a);

  std::string getUndefinedReadingName() const;
  QMap<std::string, double>* getDefaultAccuracy() const;

  bool accept(SensorControl*, Configuration::SensorFunctionalities) {
    return false;
  }

  void abortAction();

  //! connect app with laser tracker
  bool connectSensor(ConnectionConfig*);

  //! disconnect app with laser tracker
  bool disconnectSensor();

  //! laser tracker measures a point and returns an observation
  QList<Reading*> measure(MeasurementConfig* mc);

  //! stream
  oi::ValueMap readingStream(Configuration::ReadingTypes streamFormat);

  //! getConnectionState
  bool getConnectionState();

  //! return ready state of the sensor
  bool isReadyForMeasurement();

  //! sensor stats
  QMap<std::string, std::string> getSensorStats();

  //! checks if sensor is busy
  bool isBusy();

 private:
  bool isConnected;
};

#endif  // OI_DEFAULT_PLUGIN_SENSORS_UNDEFINED_SENSOR_PSEUDO_SENSOR_P_PSEUDO_SENSOR_H_
