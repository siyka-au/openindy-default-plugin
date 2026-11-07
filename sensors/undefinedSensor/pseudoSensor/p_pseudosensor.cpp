#include "p_pseudosensor.h"

#include <QtCore>

PseudoSensor::PseudoSensor() {}

PluginMetaData* PseudoSensor::getMetaData() const {
  PluginMetaData* metaData = new PluginMetaData();

  metaData->name = "PseudoSensor";
  metaData->pluginName = "OpenIndy Default Plugin";
  metaData->author = "mlux";
  metaData->description = "Simuliert einen nicht definierten Sensor";
  metaData->iid = "de.openIndy.Plugin.Sensor.v001";
  //...

  return metaData;
}

QList<Configuration::ReadingTypes>* PseudoSensor::getSupportedReadingTypes()
    const {
  QList<Configuration::ReadingTypes>* readingTypes =
      new QList<Configuration::ReadingTypes>;

  readingTypes->append(Configuration::eUndefined);

  return readingTypes;
}

QList<Configuration::SensorFunctionalities>
PseudoSensor::getSupportedSensorActions() const {
  QList<Configuration::SensorFunctionalities> a;

  return a;
}

QList<Configuration::ConnectionTypes>* PseudoSensor::getConnectionType() const {
  QList<Configuration::ConnectionTypes>* connectionTypes =
      new QList<Configuration::ConnectionTypes>;
  connectionTypes->append(Configuration::eSerial);

  return connectionTypes;
}

QMap<std::string, int>* PseudoSensor::getIntegerParameter() const {
  return NULL;
}

QMap<std::string, double>* PseudoSensor::getDoubleParameter() const {
  return NULL;
}

QMap<std::string, std::vector<std::string>>* PseudoSensor::getStringParameter()
    const {
  return NULL;
}

std::vector<std::string> PseudoSensor::selfDefinedActions() const {
  std::vector<std::string> a;
  return a;
}

bool PseudoSensor::doSelfDefinedAction(std::string a) { return true; }

std::string PseudoSensor::getUndefinedReadingName() const {
  return "pseudo reading";
}

QMap<std::string, double>* PseudoSensor::getDefaultAccuracy() const {
  QMap<std::string, double>* undefinedSigma = new QMap<std::string, double>;

  undefinedSigma->insert("pseudoSigma", 0.54);

  return undefinedSigma;
}

void PseudoSensor::abortAction() {
  // abort
}

//! laser tracker measures a point and returns an observation
QList<Reading*> PseudoSensor::measure(MeasurementConfig* mc) {
  QList<Reading*> readings;

  switch (mc->typeOfReading) {
    case Configuration::eUndefined: {
      Reading* r = new Reading();

      double randomValue =
          ((double)rand() / RAND_MAX) * (6.283185 - 0.0001) + 1.0;

      r->rUndefined.values.insert("pseudoMeasurement_1", randomValue);
      r->rUndefined.values.insert("pseudoMeasurement_2", randomValue);

      readings.append(r);

      break;
    }
    default:
      break;
  }

  if (readings.size() > 0) {
    this->lastReading.first = readings.last()->typeofReading;
    Reading* r = new Reading();
    *r = *readings.last();
    this->lastReading.second = r;
  }

  return readings;
}

oi::ValueMap PseudoSensor::readingStream(
    Configuration::ReadingTypes streamFormat) {
  oi::ValueMap r;

  double randomValue = ((double)rand() / RAND_MAX) * (6.283185 - 0.0001) + 1.0;

  r.insert("pseudoMeasurement_1", randomValue);
  r.insert("pseudoMeasurement_2", randomValue);
  QThread::msleep(100);
  return r;
}

bool PseudoSensor::getConnectionState() { return isConnected; }

bool PseudoSensor::isReadyForMeasurement() { return true; }

QMap<std::string, std::string> PseudoSensor::getSensorStats() {
  QMap<std::string, std::string> a;

  return a;
}

bool PseudoSensor::isBusy() { return false; }

//! connect app with laser tracker
bool PseudoSensor::connectSensor(ConnectionConfig* cConfig) {
  if (cConfig != NULL) {
    isConnected = true;
    QThread::msleep(1000);
    return true;
  } else {
    return false;
  }
}

//! disconnect app with laser tracker
bool PseudoSensor::disconnectSensor() {
  isConnected = false;
  QThread::msleep(1000);
  return true;
}
