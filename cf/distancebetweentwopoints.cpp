#include "distancebetweentwopoints.h"


void DistanceBetweenTwoPoints::init() {
    this->metaData.name = "DistanceBetweenTwoPoints";
    this->applicableFor.append(eScalarEntityDistanceFeature);
}

std::expected<ScalarEntityDistance, QString> DistanceBetweenTwoPoints::exec()
{
    if(this->inputElements[0].isEmpty()) {
        emit this->sendMessage("inputElements[0] is empty", MessageTypes::eWarningMessage);
        return std::unexpected(QString("DistanceBetweenTwoPoints: inputElements[0] is empty"));
    }
    if(this->inputElements[1].isEmpty()) {
        emit this->sendMessage("inputElements[1] is empty", MessageTypes::eWarningMessage);
        return std::unexpected(QString("DistanceBetweenTwoPoints: inputElements[1] is empty"));
    }
    OiVec p1 = this->inputElements[0].first().geometry->getPosition().getVector();
    OiVec p2 = this->inputElements[1].first().geometry->getPosition().getVector();
    OiVec v = p2-p1;
    double dot;
    OiVec::dot(dot, v, v);
    ScalarEntityDistance distance;
    distance.setDistance(qSqrt(dot));

    return distance;
}
