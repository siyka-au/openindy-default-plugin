#include "p_pseudotracker.h"

/*!
 * \brief PseudoTracker::init
 */
void PseudoTracker::init(){

    //set plugin meta data
    this->metaData.name = "PseudoTracker";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "mlux";
    this->metaData.description = "Simulates a laser tracker.";
    this->metaData.iid = LaserTracker_iidd;

    //set supported reading types
    this->supportedReadingTypes.append(eCartesianReading);
    this->supportedReadingTypes.append(ePolarReading);
    this->supportedReadingTypes.append(eDirectionReading);
    this->supportedReadingTypes.append(eDistanceReading);
    this->supportedReadingTypes.append(eLevelReading);
    this->supportedReadingTypes.append(eTemperatureReading);
    this->supportedReadingTypes.append(eLevelReading);

    //set supported sensor actions
    this->supportedSensorActions.append(eConnect);
    this->supportedSensorActions.append(eDisconnect);
    this->supportedSensorActions.append(eMeasure);
    this->supportedSensorActions.append(eHome);
    this->supportedSensorActions.append(eInitialize);
    this->supportedSensorActions.append(eMoveAngle);
    this->supportedSensorActions.append(eMoveXYZ);
    this->supportedSensorActions.append(eToggleSight);
    this->supportedSensorActions.append(eCompensation);
    this->supportedSensorActions.append(eMotorState);
    this->supportedSensorActions.append(eSearch);

    //this simulator only fabricates readings on demand; there is no live stream
    this->supportedAcquisitionModes.append(AcquisitionMode::eDiscrete);

    //set supported connection types
    this->supportedConnectionTypes.append(eNetworkConnection);
    this->supportedConnectionTypes.append(eSerialConnection);

    //set double parameter
    this->doubleParameters.insert("lambda [mm]",0.000403);
    this->doubleParameters.insert("mu",0.000005);
    this->doubleParameters.insert("ex [mm]",0.0000122);
    this->doubleParameters.insert("by [mm]",0.0000654);
    this->doubleParameters.insert("bz [mm]",0.0000974);
    this->doubleParameters.insert("alpha [arc sec]",0.128);
    this->doubleParameters.insert("gamma [arc sec]",0.079);
    this->doubleParameters.insert("Aa1 [arc sec]",0.064);
    this->doubleParameters.insert("Ba1 [arc sec]",0.080);
    this->doubleParameters.insert("Aa2 [arc sec]",0.073);
    this->doubleParameters.insert("Ba2 [arc sec]",0.090);
    this->doubleParameters.insert("Ae0 [arc sec]",0.223);
    this->doubleParameters.insert("Ae1 [arc sec]",0.152);
    this->doubleParameters.insert("Be1 [arc sec]",0.183);
    this->doubleParameters.insert("Ae2 [arc sec]",0.214);
    this->doubleParameters.insert("Be2 [arc sec]",0.179);

    //set string parameter
    this->stringParameters.insert("active probe", "0.5''");
    this->stringParameters.insert("active probe", "1.0''");
    this->stringParameters.insert("active probe", "1.5''");
    this->stringParameters.insert("reading type", "polar");
    this->stringParameters.insert("reading type", "cartesian");

    //set self defined actions
    this->selfDefinedActions.append("echo(Alt+E)");
    this->selfDefinedActions.append("stopMeasure"); // e.g. finish scanning
    this->selfDefinedActions.append("toggle return readings"); // for tests, if set to false then OpenIndy responds with an incorrect measurement

    //set default accuracy
    this->defaultAccuracy.sigmaAzimuth = 0.000001570;
    this->defaultAccuracy.sigmaZenith = 0.000001570;
    this->defaultAccuracy.sigmaDistance = 0.000025;
    this->defaultAccuracy.sigmaXyz.setAt(0, 0.000025);
    this->defaultAccuracy.sigmaXyz.setAt(1, 0.000025);
    this->defaultAccuracy.sigmaXyz.setAt(2, 0.000025);
    this->defaultAccuracy.sigmaTemp = 0.5;
    this->defaultAccuracy.sigmaI = 0.000001570;
    this->defaultAccuracy.sigmaJ = 0.000001570;
    this->defaultAccuracy.sigmaK = 0.000001570;

    //general tracker inits
    this->myAzimuth = 0.00001;
    this->myZenith = 0.00001;
    this->myDistance =0.000001;
    this->myMotor = false;
    this->myInit = false;
    this->myCompIt = false;
    this->isConnected = false;
    this->side = 1;

    this->returnReading = true;

}

/*!
 * \brief PseudoTracker::doSelfDefinedAction
 * \param action
 * \return
 */
bool PseudoTracker::doSelfDefinedAction(const QString &action){
    qDebug() << "PseudoTracker::doSelfDefinedAction(): " << action;
    if(action == "echo"){
        emit this->sensorMessage(action, eInformationMessage, eMessageBoxMessage);
    } else if (action == "stopMeasure") {
        this->isScanning = false;
        emit this->sensorMessage("try to stop / finish measurement", eInformationMessage, eConsoleMessage);
    } else if(action == "toggle return readings") {
        this->returnReading = !this->returnReading; // toggle
        emit this->sensorMessage(QString("return readings: %1").arg(this->returnReading.load() ? "true" : "false"), eInformationMessage, eConsoleMessage);
    }
    return true;
}

/*!
 * \brief PseudoTracker::abortAction
 */
bool PseudoTracker::abortAction(){
    return false;
}

/*!
 * \brief PseudoTracker::connectSensor
 * \return
 */
bool PseudoTracker::connectSensor(){
    this->isConnected = true;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::disconnectSensor
 * \return
 */
bool PseudoTracker::disconnectSensor(){
    this->isConnected = false;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::initialize
 * \return
 */
bool PseudoTracker::initialize(){
    this->myInit = true;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::move
 * \param azimuth
 * \param zenith
 * \param distance
 * \param isRelative
 * \return
 */
bool PseudoTracker::move(const double &azimuth, const double &zenith, const double &distance, const bool &isRelative){
    this->myAzimuth = azimuth;
    this->myZenith = zenith;
    this->myDistance = distance;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::move
 * \param x
 * \param y
 * \param z
 * \return
 */
bool PseudoTracker::move(const double &x, const double &y, const double &z){
    this->myAzimuth = qAtan2(y,x);
    this->myDistance = qSqrt(x*x+y*y+z*z);
    this->myZenith = this->myDistance == 0. ? M_PI / 2. : acos(z/myDistance);
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::home
 * \return
 */
bool PseudoTracker::home(){
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::changeMotorState
 * \return
 */
bool PseudoTracker::changeMotorState(){
    this->myMotor = !this->myMotor;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::toggleSightOrientation
 * \return
 */
bool PseudoTracker::toggleSightOrientation(){
    if(this->side == 1){
        this->side = 2;
    }else{
        this->side = 1;
    }
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::compensation
 * \return
 */
bool PseudoTracker::compensation() {
    QThread::msleep(5000);
    this->myCompIt = true;
    return true;
}

/*!
 * \brief PseudoTracker::measure
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measure(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    if(!this->returnReading) {
        return readings;
    }

    const int faceCount = mConfig.getMeasureTwoSides() ? 2 : 1;

    int scanPointCount = mConfig.getMaxObservations();
    const bool meaurementTypeScan = mConfig.getMeasurementType() == MeasurementTypes::eScanDistanceDependent_MeasurementType
            || mConfig.getMeasurementType() == MeasurementTypes::eScanTimeDependent_MeasurementType;
    this->measureTime =  mConfig.getMeasurementType() == MeasurementTypes::eScanTimeDependent_MeasurementType ? mConfig.getTimeInterval() * 1000 : 1000;
    this->isScanning = meaurementTypeScan;

    do {
        for(int face=0; face<faceCount; face++) {

            switch (getReadingType(mConfig)) {
            case ePolarReading:{
                readings += measurePolar(mConfig);
                break;
            }case eDistanceReading:{
                readings += measureDistance(mConfig);
                break;
            }case eDirectionReading:{
                readings += measureDirection(mConfig);
                break;
            }case eCartesianReading:{
                readings += measureCartesian(mConfig);
                break;
            }case eLevelReading:{
                readings += measureLevel(mConfig);
                break;
            }
            }

            if(mConfig.getMeasureTwoSides() && face<(faceCount -1)) {
                this->toggleSightOrientation();
            }

        }
        qDebug()<< "isScanning: " << isScanning;

    } while(meaurementTypeScan && scanPointCount-- > 1 && this->isScanning);
    this->isScanning = false;

    if(readings.size() > 0){

        //delete old last reading
        if(!this->lastReading.second.isNull()){
            delete this->lastReading.second;
        }

        this->lastReading.first = readings.last()->getTypeOfReading();
        QPointer<Reading> r = new Reading(*readings.last().data());
        this->lastReading.second = r;

    }

    return readings;

}

/*!
 * \brief PseudoTracker::readingStream
 * \param streamFormat
 * \return
 */
QVariantMap PseudoTracker::readingStream(const ReadingTypes &streamFormat){

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double distance = 0.0;
    double azimuth = 0.0;
    double zenith = 0.0;

    QVariantMap m;

    QPointer<Reading> r(NULL);

    switch (streamFormat) {
    case ePolarReading:{

        const ReadingPolar rPolar = this->simulateAim();

        r = new Reading(rPolar);

        m.insert("azimuth", rPolar.azimuth);
        m.insert("zenith", rPolar.zenith);
        m.insert("distance", rPolar.distance);

        break;

    }case eCartesianReading:{

        const ReadingPolar rPolar = this->simulateAim();

        r = new Reading(rPolar);

        m.insert("x", r->getCartesianReading().xyz.getAt(0));
        m.insert("y", r->getCartesianReading().xyz.getAt(1));
        m.insert("z", r->getCartesianReading().xyz.getAt(2));

        break;

    }case eDistanceReading:{

        ReadingDistance rDistance;
        rDistance.distance = this->myDistance;
        rDistance.isValid = true;

        r = new Reading(rDistance);

        m.insert("distance", r->getDistanceReading().distance);

        break;

    }case eDirectionReading:{

        ReadingDirection rDirection;
        rDirection.azimuth = this->myAzimuth;
        rDirection.zenith = this->myZenith;
        rDirection.isValid = true;

        r = new Reading(rDirection);

        m.insert("azimuth", r->getDirectionReading().azimuth);
        m.insert("zenith", r->getDirectionReading().zenith);

        break;

    }case eTemperatureReading:{
        break;
    }case eLevelReading:{
        break;
    }case eUndefinedReading:{
        break;
    }default:
        break;
    }

    //delete old last reading
    if(!this->lastReading.second.isNull()){
        delete this->lastReading.second;
    }

    //check reading
    if(r.isNull()){
        return m;
    }

    this->lastReading.first = r->getTypeOfReading();
    this->lastReading.second = r;

    QThread::msleep(300);

    return m;

}

/*!
 * \brief PseudoTracker::getConnectionState
 * \return
 */
bool PseudoTracker::getConnectionState(){
    return isConnected;
}

/*!
 * \brief PseudoTracker::isReadyForMeasurement
 * \return
 */
bool PseudoTracker::getIsReadyForMeasurement(){
    return true;
}

/*!
 * \brief PseudoTracker::getSensorStats
 * \return
 */
QMap<QString, QString> PseudoTracker::getSensorStatus(){

    QMap<QString, QString> stats;

    stats.insert("connected",QString::number(isConnected));
    stats.insert("side", QString::number(side));
    stats.insert("myAzimuth", QString::number(myAzimuth));
    stats.insert("myZenith", QString::number(myZenith));
    stats.insert("myDistance", QString::number(myDistance));
    stats.insert("myMotor", QString::number(myMotor));
    stats.insert("myInit", QString::number(myInit));
    stats.insert("myCompIt", QString::number(myCompIt));

    QThread::msleep(300);

    return stats;

}

/*!
 * \brief PseudoTracker::isBusy
 * \return
 */
bool PseudoTracker::getIsBusy(){
    return false;
}

/*!
 * \brief PseudoTracker::measurePolar
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measurePolar(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    const ReadingPolar rPolar = this->simulateAim();

    QPointer<Reading> p = new Reading(rPolar);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QVariant td =  mConfig.getTransientData("isDummyPoint");
    if(td.isValid()) {
        p->setProperty("isDummyPoint", td);
    } else {
        p->setProperty("isDummyPoint", false);
    }

    QThread::msleep(this->measureTime);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::measureDistance
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measureDistance(const MeasurementConfig &mConfig){

    Q_UNUSED(mConfig)

    QList<QPointer<Reading> > readings;

    //share the same modelled polar draw as every other reading type instead
    //of an independent, unmodelled noise stub - distance-only readings
    //should scatter and report sigma consistently with a polar measurement
    //of the same aim
    const ReadingPolar rPolar = this->simulateAim();

    ReadingDistance rDistance;
    rDistance.distance = rPolar.distance;
    rDistance.sigmaDistance = rPolar.sigmaDistance;
    rDistance.isValid = true;

    QPointer<Reading> p = new Reading(rDistance);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::measureDirection
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measureDirection(const MeasurementConfig &mConfig){

    Q_UNUSED(mConfig)

    QList<QPointer<Reading> > readings;

    const ReadingPolar rPolar = this->simulateAim();

    ReadingDirection rDirection;
    rDirection.azimuth = rPolar.azimuth;
    rDirection.zenith = rPolar.zenith; // was myAzimuth - zenith computed from azimuth
    rDirection.sigmaAzimuth = rPolar.sigmaAzimuth;
    rDirection.sigmaZenith = rPolar.sigmaZenith;
    rDirection.isValid = true;

    QPointer<Reading> p = new Reading(rDirection);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::measureCartesian
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measureCartesian(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    //build from a noisy polar draw and let Reading's own polar->cartesian
    //propagation (Reading::errorPropagationPolarToCartesian, a proper
    //Jacobian) derive xyz and sigmaXyz, instead of adding independent,
    //unmodelled per-axis noise on top of the noise-free aim
    const Reading polarReading(this->simulateAim());
    const ReadingCartesian rCartesian = polarReading.getCartesianReading();

    QPointer<Reading> p = new Reading(rCartesian);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QVariant td =  mConfig.getTransientData("isDummyPoint");
    if(td.isValid()) {
        p->setProperty("isDummyPoint", td);
    } else {
        p->setProperty("isDummyPoint", false);
    }

    QThread::msleep(this->measureTime);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::simulateAim
 * Draws one noisy polar observation of the current aim (myAzimuth/myZenith/
 * myDistance) using this instance's configured error terms, with sigma set
 * from the model's own estimate of the noise it just applied - see
 * TrackerErrorModel. Replaces the old randomX/randomNorm/randomTriangular/
 * noisyPolarReading, which duplicated this same error model
 * (Hughes, Sun, Forbes, Lewis 2010) verbatim inside this driver.
 * \return
 */
ReadingPolar PseudoTracker::simulateAim() const{

    TrackerErrorTerms terms;
    const QMap<QString, double> params = this->sensorConfiguration.getDoubleParameter();
    terms.lambdaMm = params.value("lambda [mm]");
    terms.mu = params.value("mu");
    terms.exMm = params.value("ex [mm]");
    terms.byMm = params.value("by [mm]");
    terms.bzMm = params.value("bz [mm]");
    terms.alphaArcsec = params.value("alpha [arc sec]");
    terms.gammaArcsec = params.value("gamma [arc sec]");
    terms.Aa1Arcsec = params.value("Aa1 [arc sec]");
    terms.Ba1Arcsec = params.value("Ba1 [arc sec]");
    terms.Aa2Arcsec = params.value("Aa2 [arc sec]");
    terms.Ba2Arcsec = params.value("Ba2 [arc sec]");
    terms.Ae0Arcsec = params.value("Ae0 [arc sec]");
    terms.Ae1Arcsec = params.value("Ae1 [arc sec]");
    terms.Be1Arcsec = params.value("Be1 [arc sec]");
    terms.Ae2Arcsec = params.value("Ae2 [arc sec]");
    terms.Be2Arcsec = params.value("Be2 [arc sec]");

    const TrackerErrorModel model(terms);
    return model.apply(this->myAzimuth, this->myZenith, this->myDistance);

}

bool PseudoTracker::search() {
    emit this->sensorMessage("search", eInformationMessage, eConsoleMessage);
    QThread::msleep(1000);
    return true;
}

QList<QPointer<Reading> > PseudoTracker::measureLevel(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    ReadingLevel rLevel;
    rLevel.i = ((double) rand()/RAND_MAX) / 1000.;
    rLevel.j = ((double) rand()/RAND_MAX) / 1000.;
    rLevel.k = sqrt(1. - pow(rLevel.i, 2) - pow(rLevel.j, 2));

    rLevel.sigmaI = defaultAccuracy.sigmaI;
    rLevel.sigmaJ = defaultAccuracy.sigmaJ;
    rLevel.sigmaK = defaultAccuracy.sigmaK;
    rLevel.isValid = true;

    QPointer<Reading> p = new Reading(rLevel);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}
