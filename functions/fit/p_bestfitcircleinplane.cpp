#include "p_bestfitcircleinplane.h"

/*!
 * \brief BestFitCircleInPlane::init
 */
void BestFitCircleInPlane::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCircleInPlane";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2 %3 %4")
            .arg("This function calculates an adjusted circle.")
            .arg("The points are registered into a best fit plane first and afterward a 2D circle is approximated inside the plane.")
            .arg("You can input as many points as you want which are then used to find the best fit circle.")
            .arg("There will be no plane feature/calculations vissible, the calculation is performed inside the function");
    this->metaData.iid = FitFunction_iidd;

    //D12/Stage 7c-ii: consolidates BestFitCircleInPlane (raw Observations)
    //and BestFitCircleInPlaneFromPoints (Points, but missing roleName
    //"default" on its primary input, so it could never actually be
    //tag-resolved either) into one tag-resolved, Point-consuming
    //declaration.
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least three points to calculate the best fit circle.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    param1.roleName = "default";
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Dummy point to indicate circle normal.";
    param2.infinite = true;
    param2.typeOfElement = ePointElement;
    param2.roleName = "dummyPoint";
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(eCircleFeature);

}

/*!
 * \brief BestFitCircleInPlane::exec
 * \param circle
 * \return
 */
bool BestFitCircleInPlane::exec(Circle &circle){
    this->statistic.reset();
    return this->setUpResult(circle);
}

/*!
 * \brief BestFitCircleInPlane::setUpResult
 * \param circle
 * \return
 */
bool BestFitCircleInPlane::setUpResult(Circle &circle){

    if(!this->inputElements.contains(0) || this->inputElements[0].isEmpty()){
        emit this->sendMessage(QString("No points tagged for circle %1").arg(circle.getFeatureName()), eWarningMessage);
        return false;
    }

    QList<IdPoint> usablePoints;
    QList<IdPoint> points;
    {
        QList<QPointer<Point> > allUsablePoints;
        QList<QPointer<Point> > inputPoints;
        foreach(const InputElement &element, this->inputElements[0]){
            if(element.point.isNull() || !element.point->getIsSolved()){
                this->setIsUsed(0, element.id, false);
                continue;
            }
            allUsablePoints.append(element.point);
            this->setIsUsed(0, element.id, element.shouldBeUsed);
            if(element.shouldBeUsed){
                inputPoints.append(element.point);
            }
        }

        if(inputPoints.size() < 3){
            emit this->sendMessage(QString("Not enough valid points to fit the circle %1").arg(circle.getFeatureName()), eWarningMessage);
            return false;
        }

        foreach(const QPointer<Point> &p, allUsablePoints) {
            IdPoint point;
            point.id = p->getId();
            point.xyz = p->getPosition().getVectorH();
            usablePoints.append(point);
        }
        foreach(const QPointer<Point> &p, inputPoints) {
            IdPoint point;
            point.id = p->getId();
            point.xyz = p->getPosition().getVectorH();
            points.append(point);
        }
    }

    return bestFitCircleInPlane(this, circle, points, usablePoints);
}
