#include "p_linefromplanenormal.h"

/*!
 * \brief LineFromPlaneNormal::init
 */
void LineFromPlaneNormal::init(){

    //set plugin meta data
    this->metaData.name = "LineFromPlaneNormal";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "oi";
    this->metaData.description = QString("%1 %2")
            .arg("This function creates a line along the normal of a plane, through the plane's position.")
            .arg("The line follows the plane: change or re-measure the plane and the line updates with it.");
    this->metaData.iid = ConstructFunction_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select the plane whose normal defines the line.";
    param1.infinite = false;
    param1.typeOfElement = ePlaneElement;
    this->neededElements.append(param1);

    //set applicable for
    this->applicableFor.append(eLineFeature);

}

/*!
 * \brief LineFromPlaneNormal::exec
 * \param line
 * \return
 */
bool LineFromPlaneNormal::exec(Line &line){
    return this->setUpResult(line);
}

/*!
 * \brief LineFromPlaneNormal::setUpResult
 * \param line
 * \return
 */
bool LineFromPlaneNormal::setUpResult(Line &line){

    if(!this->inputElements.contains(0) || this->inputElements[0].size() != 1){
        emit this->sendMessage(QString("No plane set for line %1").arg(line.getFeatureName()), eWarningMessage);
        return false;
    }

    const InputElement &element = this->inputElements[0].first();

    //an unsolved plane has no meaningful normal yet, so the line it would
    //define would be meaningless too
    if(element.plane.isNull() || !element.plane->getIsSolved()){
        this->setIsUsed(0, element.id, false);
        emit this->sendMessage(QString("The plane for line %1 is not solved").arg(line.getFeatureName()), eWarningMessage);
        return false;
    }

    this->setIsUsed(0, element.id, true);

    //the plane's own position and normal define the line directly - no fit,
    //no approximation, just the geometry that is already there
    Position linePosition = element.plane->getPosition();
    Direction lineDirection = element.plane->getDirection();

    line.setLine(linePosition, lineDirection);

    return true;

}
