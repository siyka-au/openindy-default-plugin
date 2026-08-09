#include "p_pointfromplaneorigin.h"

/*!
 * \brief PointFromPlaneOrigin::init
 */
void PointFromPlaneOrigin::init(){

    //set plugin meta data
    this->metaData.name = "PointFromPlaneOrigin";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "oi";
    this->metaData.description = QString("%1 %2")
            .arg("This function creates a point at the position of a plane, i.e. the centroid its fit solved to.")
            .arg("The point follows the plane: change or re-measure the plane and the point updates with it.");
    this->metaData.iid = ConstructFunction_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select the plane whose position defines the point.";
    param1.infinite = false;
    param1.typeOfElement = ePlaneElement;
    this->neededElements.append(param1);

    //set applicable for
    this->applicableFor.append(ePointFeature);

}

/*!
 * \brief PointFromPlaneOrigin::exec
 * \param point
 * \return
 */
bool PointFromPlaneOrigin::exec(Point &point){
    return this->setUpResult(point);
}

/*!
 * \brief PointFromPlaneOrigin::setUpResult
 * \param point
 * \return
 */
bool PointFromPlaneOrigin::setUpResult(Point &point){

    if(!this->inputElements.contains(0) || this->inputElements[0].size() != 1){
        emit this->sendMessage(QString("No plane set for point %1").arg(point.getFeatureName()), eWarningMessage);
        return false;
    }

    const InputElement &element = this->inputElements[0].first();

    if(element.plane.isNull() || !element.plane->getIsSolved()){
        this->setIsUsed(0, element.id, false);
        emit this->sendMessage(QString("The plane for point %1 is not solved").arg(point.getFeatureName()), eWarningMessage);
        return false;
    }

    this->setIsUsed(0, element.id, true);

    Position pointPosition = element.plane->getPosition();
    point.setPoint(pointPosition);

    return true;

}
