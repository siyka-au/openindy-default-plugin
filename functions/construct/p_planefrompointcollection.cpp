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

    //exactly one collection, not N points: that is the whole point of this
    //function, and why infinite is false here
    NeededElement param1;
    param1.description = "Select the point collection to fit the plane to.";
    param1.infinite = false;
    param1.typeOfElement = ePointCollectionElement;
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

    //exactly one input element, and it must resolve to a collection
    if(!this->inputElements.contains(0) || this->inputElements[0].size() != 1){
        emit this->sendMessage(QString("No point collection set for plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    const InputElement &element = this->inputElements[0].first();
    const PointCollection *collection = element.asPointCollection();
    if(collection == nullptr){
        emit this->sendMessage(QString("Input of plane %1 is not a point collection").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    //the collection decides what counts as usable - an unsolved point or a
    //cloud point excluded by segmentation is simply not in here
    const QList<Position> positions = collection->collectionPoints();

    //three is the geometric minimum for a plane. It is not sufficient:
    //three collinear points satisfy the count and still cannot define one,
    //which is why the real check is whether the fit below actually solves.
    if(positions.size() < 3){
        emit this->sendMessage(QString("Not enough points in the collection to fit the plane %1 (have %2, need at least 3)")
                                .arg(plane.getFeatureName()).arg(positions.size()), eWarningMessage);
        this->setIsUsed(0, element.id, false);
        return false;
    }

    this->setIsUsed(0, element.id, true);

    QList<IdPoint> points;
    for(int i = 0; i < positions.size(); ++i){
        IdPoint point;
        //a collection point has no feature id of its own - a cloud point
        //never had one, and a group's points are addressed through the
        //group rather than individually here
        point.id = i;
        point.xyz = positions.at(i).getVector();
        points.append(point);
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
