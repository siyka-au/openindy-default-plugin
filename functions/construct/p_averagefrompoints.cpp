#include "p_averagefrompoints.h"

/*!
 * \brief AverageFromPoints::init
 */
void AverageFromPoints::init(){

    //set plugin meta data
    this->metaData.name = "AverageFromPoints";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "oi";
    this->metaData.description = QString("%1 %2")
            .arg("This function creates a point at the arithmetic mean position of as many points as you give it.")
            .arg("Use it to combine repeated shots of one physical target - e.g. a tracker nest re-shot with a total station - into one derived point.");
    this->metaData.iid = ConstructFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select as many points as you want to average.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    param1.roleName = "default";
    this->neededElements.append(param1);

    //set applicable for
    this->applicableFor.append(ePointFeature);

}

/*!
 * \brief AverageFromPoints::exec
 * \param point
 * \return
 */
bool AverageFromPoints::exec(Point &point){
    return this->setUpResult(point);
}

/*!
 * \brief AverageFromPoints::setUpResult
 * \param point
 * \return
 */
bool AverageFromPoints::setUpResult(Point &point){

    if(!this->inputElements.contains(0) || this->inputElements[0].isEmpty()){
        emit this->sendMessage(QString("No points to average for point %1").arg(point.getFeatureName()), eWarningMessage);
        return false;
    }

    OiVec sum(3);
    sum.setAt(0, 0.0);
    sum.setAt(1, 0.0);
    sum.setAt(2, 0.0);
    int usedCount = 0;

    foreach(const InputElement &element, this->inputElements[0]){

        //degenerate points pass the "at least one" hint just like a
        //collinear set passes a plane's minimum count - reject inside
        //exec(), not by declared count (domain-model.md S9.4)
        if(element.point.isNull() || !element.point->getIsSolved() || !element.shouldBeUsed){
            this->setIsUsed(0, element.id, false);
            continue;
        }

        this->setIsUsed(0, element.id, true);

        const OiVec &xyz = element.point->getPosition().getVector();
        sum.setAt(0, sum.getAt(0) + xyz.getAt(0));
        sum.setAt(1, sum.getAt(1) + xyz.getAt(1));
        sum.setAt(2, sum.getAt(2) + xyz.getAt(2));
        ++usedCount;

    }

    if(usedCount == 0){
        emit this->sendMessage(QString("No solved points to average for point %1").arg(point.getFeatureName()), eWarningMessage);
        return false;
    }

    sum.setAt(0, sum.getAt(0) / usedCount);
    sum.setAt(1, sum.getAt(1) / usedCount);
    sum.setAt(2, sum.getAt(2) / usedCount);

    point.setPoint(Position(sum));

    return true;

}
