#include "p_bestfitplane.h"

/*!
 * \brief BestFitPlane::init
 */
void BestFitPlane::init(){

    //set plugin meta data
    this->metaData.name = "BestFitPlane";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted plane.")
            .arg("You can input as many points as you want which are then used to find the best fit plane.");
    this->metaData.iid = FitFunction_iidd;

    //D12/domain-model.md S2/S3: only a Point is ever measured, and a fit's
    //point input resolves to a query over MeasurementIntent tags matching
    //this NeededElement's roleName (OiJob::resolvePointIntent), not a
    //stored binding. Stage 7c-ii consolidates what used to be two
    //functions (this one over raw Observations, PlaneFromPoints/
    //PlaneFromPointCollection over Points) into this single Point-
    //consuming, tag-resolved declaration - following the shape
    //PlaneFromPointCollection established in Stage 7f.
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least three points to calculate the best fit plane.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    param1.roleName = "default";
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Dummy point to indicate plane normal.";
    param2.infinite = true;
    param2.typeOfElement = ePointElement;
    param2.roleName = "dummyPoint";
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(ePlaneFeature);

}

/*!
 * \brief BestFitPlane::exec
 * \param plane
 * \return
 */
bool BestFitPlane::exec(Plane &plane){
    this->statistic.reset();
    return this->setUpResult(plane);
}

/*!
 * \brief BestFitPlane::setUpResult
 * \param plane
 * \return
 */
bool BestFitPlane::setUpResult(Plane &plane){

    if(!this->inputElements.contains(0) || this->inputElements[0].isEmpty()){
        emit this->sendMessage(QString("No points tagged for plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    //only solved points count as usable; among those, only shouldBeUsed
    //ones enter the fit itself - the same used/usable split
    //filterObservations used to provide for the observation-based version
    QList<Position> usedPositions;
    QList<IdPoint> points;
    foreach(const InputElement &element, this->inputElements[0]){

        if(element.point.isNull() || !element.point->getIsSolved()){
            this->setIsUsed(0, element.id, false);
            continue;
        }
        this->setIsUsed(0, element.id, element.shouldBeUsed);
        if(!element.shouldBeUsed){
            continue;
        }

        usedPositions.append(element.point->getPosition());

        IdPoint point;
        point.id = element.id;
        point.xyz = element.point->getPosition().getVector();
        points.append(point);

    }

    //three is the geometric minimum for a plane. It is not sufficient:
    //three collinear points satisfy the count and still cannot define one,
    //which is why the real check is whether the fit below actually solves
    //(bestFitPlane's own degeneracy check, fitfunction.h) - a declared
    //minimum is a hint, never a gate (domain-model.md S9.4).
    if(usedPositions.size() < 3){
        emit this->sendMessage(QString("Not enough points tagged for plane %1 (have %2, need at least 3)")
                                .arg(plane.getFeatureName()).arg(usedPositions.size()), eWarningMessage);
        return false;
    }

    OiVec centroid(3);
    OiVec n(3);
    double eVal = 0.0;
    if(!bestFitPlane(centroid, n, eVal, points)){
        emit this->sendMessage(QString("Cannot fit plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    OiVec direction(3);

    if(hasDummyPoint(this)) {
        // computing plane normale by dummy point
        OiVec dummyPoint;
        getDummyPoint(dummyPoint, this);
        direction = dummyPoint - centroid;
        direction.normalize();
    } else {
        //check that the normal vector of the plane is defined by the first three points A, B and C (cross product)
        OiVec ab = usedPositions.at(1).getVector() - usedPositions.at(0).getVector();
        OiVec ac = usedPositions.at(2).getVector() - usedPositions.at(0).getVector();
        OiVec::cross(direction, ab, ac);
        direction.normalize();
    }

    rectifyNormalToDirection(n, direction);

    //calculate smallest distance of the plane from the origin
    double dOrigin = n.getAt(0) * centroid.getAt(0) + n.getAt(1) * centroid.getAt(1) + n.getAt(2) * centroid.getAt(2);

    //calculate display residuals for every solved point, whether or not it
    //was actually used in the fit (matches the observation-based version's
    //allUsableObservations behaviour)
    foreach(const InputElement &element, this->inputElements[0]){
        if(element.point.isNull() || !element.point->getIsSolved()){
            continue;
        }

        const OiVec pos = element.point->getPosition().getVector();
        double distance = n.getAt(0) * pos.getAt(0) + n.getAt(1) * pos.getAt(1) + n.getAt(2) * pos.getAt(2) - dOrigin;
        OiVec v_plane = distance * n;

        Function::addDisplayResidual(element.id, v_plane.getAt(0), v_plane.getAt(1), v_plane.getAt(2), distance);
    }

    //set result
    Position planePosition;
    planePosition.setVector(centroid);
    Direction planeDirection;
    planeDirection.setVector(n);
    plane.setPlane(planePosition, planeDirection);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(eVal/(usedPositions.size()-3.0)));
    plane.setStatistic(this->statistic);

    return true;

}
