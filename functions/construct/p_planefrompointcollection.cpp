#include "p_planefrompointcollection.h"

/*!
 * \brief PlaneFromPointCollection::init
 */
void PlaneFromPointCollection::init(){

    //set plugin meta data
    this->metaData.name = "PlaneFromPointCollection";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "oi";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates a best fit plane from a point collection.")
            .arg("Points added to the collection are picked up automatically - the function itself never has to be re-bound.");
    this->metaData.iid = ConstructFunction_iidd;

    //D12: one declaration, resolved to as many tagged points as exist -
    //infinite=true is what keeps this function's own input list from
    //growing "by construction" as points arrive, the same guarantee
    //PointCollection binding used to provide.
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least three points to calculate the best fit plane.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    param1.roleName = "default";
    this->neededElements.append(param1);

    //set applicable for
    this->applicableFor.append(ePlaneFeature);

}

/*!
 * \brief PlaneFromPointCollection::exec
 * \param plane
 * \return
 */
bool PlaneFromPointCollection::exec(Plane &plane){
    return this->setUpResult(plane);
}

/*!
 * \brief PlaneFromPointCollection::setUpResult
 * \param plane
 * \return
 */
bool PlaneFromPointCollection::setUpResult(Plane &plane){

    if(!this->inputElements.contains(0) || this->inputElements[0].isEmpty()){
        emit this->sendMessage(QString("No points tagged for plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    //only solved points count as usable - an unsolved (e.g. not yet
    //re-measured) point is simply not in here, same as the retired
    //collection's own filtering did
    QList<Position> positions;
    QList<IdPoint> points;
    foreach(const InputElement &element, this->inputElements[0]){

        if(element.point.isNull() || !element.point->getIsSolved() || !element.shouldBeUsed){
            this->setIsUsed(0, element.id, false);
            continue;
        }
        this->setIsUsed(0, element.id, true);

        positions.append(element.point->getPosition());

        IdPoint point;
        point.id = element.id;
        point.xyz = element.point->getPosition().getVector();
        points.append(point);

    }

    //three is the geometric minimum for a plane. It is not sufficient:
    //three collinear points satisfy the count and still cannot define one,
    //which is why the real check is whether the fit below actually solves.
    if(positions.size() < 3){
        emit this->sendMessage(QString("Not enough points tagged for plane %1 (have %2, need at least 3)")
                                .arg(plane.getFeatureName()).arg(positions.size()), eWarningMessage);
        return false;
    }

    OiVec centroid(3);
    OiVec n(3);
    double eVal = 0.0;
    if(!bestFitPlane(centroid, n, eVal, points)){
        //this is where a degenerate arrangement (collinear points) is
        //actually caught - the count check above cannot see it
        emit this->sendMessage(QString("Cannot fit plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    //orient the normal by the first three points, same convention as
    //PlaneFromPoints so the two produce comparable results
    OiVec ab = positions.at(1).getVector() - positions.at(0).getVector();
    OiVec ac = positions.at(2).getVector() - positions.at(0).getVector();
    OiVec direction(3);
    direction.normalize();
    OiVec::cross(direction, ab, ac);
    rectifyNormalToDirection(n, direction);

    //set result
    Position planePosition;
    planePosition.setVector(centroid);
    Direction planeDirection;
    planeDirection.setVector(n);
    plane.setPlane(planePosition, planeDirection);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(eVal / (positions.size() - 3.0)));
    plane.setStatistic(this->statistic);

    return true;

}
