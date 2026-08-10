#include "p_bestfitline.h"

void BestFitLine::init(){

    //set plugin meta data
    this->metaData.name = "BestFitLine";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted line.")
            .arg("You can input as many points as you want which are then used to find the best fit 3D line.");
    this->metaData.iid = FitFunction_iidd;

    //D12/Stage 7c-ii: consolidates BestFitLine (raw Observations) and
    //LineFromPoints (Points, not tag-resolved) into one tag-resolved,
    //Point-consuming declaration - see p_bestfitplane.cpp for the pattern.
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least two points to calculate the best fit line.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    param1.roleName = "default";
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eLineFeature);

}

/*!
 * \brief BestFitLine::exec
 * \param line
 * \return
 */
bool BestFitLine::exec(Line &line){
    this->statistic.reset();
    return this->setUpResult(line);
}

/*!
 * \brief BestFitLine::setUpResult
 * Set up result and statistic for type plane
 * \param p
 */
bool BestFitLine::setUpResult(Line &line){

    if(!this->inputElements.contains(0) || this->inputElements[0].isEmpty()){
        emit this->sendMessage(QString("No points tagged for line %1").arg(line.getFeatureName()), eWarningMessage);
        return false;
    }

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
    if(inputPoints.size() < 2){
        emit this->sendMessage(QString("Not enough valid points to fit the line %1").arg(line.getFeatureName()), eWarningMessage);
        return false;
    }

    //centroid
    OiVec centroid(3);
    foreach(const QPointer<Point> &point, inputPoints){
        centroid = centroid + point->getPosition().getVector();
    }
    centroid = centroid * (1.0/inputPoints.size());

    //principle component analysis
    OiMat a(inputPoints.size(), 3);
    for(int i = 0; i < inputPoints.size(); i++){
        const OiVec pos = inputPoints.at(i)->getPosition().getVector();
        a.setAt(i, 0, pos.getAt(0) - centroid.getAt(0));
        a.setAt(i, 1, pos.getAt(1) - centroid.getAt(1));
        a.setAt(i, 2, pos.getAt(2) - centroid.getAt(2));
    }
    OiMat ata = a.t() * a;
    OiMat u(3,3);
    OiVec d(3);
    OiMat v(3,3);
    ata.svd(u, d, v);

    //get largest eigenvector which is r vector and v^T * v as sum of the 2 smaller eigenvectors
    int eigenIndex = -1;
    double eVal = 0.0;
    double vtv = 0.0;
    for(int i = 0; i < d.getSize(); i++){
        if(d.getAt(i) > eVal || i == 0){
            eVal = d.getAt(i);
            eigenIndex = i;
        }else{
            vtv += d.getAt(i);
        }
    }
    OiVec r(3);
    u.getCol(r, eigenIndex);
    r.normalize();

    //check that the orientation of the line is from first to second point
    OiVec pos1 = inputPoints.at(0)->getPosition().getVector();
    OiVec pos2 = inputPoints.at(1)->getPosition().getVector();
    OiVec direction = pos2 - pos1;
    direction.normalize();
    rectifyNormalToDirection(r, direction);

    //calculate display residuals for each usable point
    foreach(const QPointer<Point> &point, allUsablePoints){
        double distance = 0.0;
        OiVec v_line(3);
        const OiVec pos = point->getPosition().getVector();
        //calculate perpendicular
        v_line.setAt(0, pos.getAt(0) - centroid.getAt(0));
        v_line.setAt(1, pos.getAt(1) - centroid.getAt(1));
        v_line.setAt(2, pos.getAt(2) - centroid.getAt(2));
        OiVec::dot(distance, r, v_line);
        v_line = centroid + distance * r;

        //calculate residual vector
        v_line.setAt(0, pos.getAt(0) - v_line.getAt(0));
        v_line.setAt(1, pos.getAt(1) - v_line.getAt(1));
        v_line.setAt(2, pos.getAt(2) - v_line.getAt(2));

        //set up display residual
        double dot;
        OiVec::dot(dot, v_line, v_line);
        addDisplayResidual(point->getId(), v_line.getAt(0), v_line.getAt(1), v_line.getAt(2), qSqrt(dot));

    }

    //set result
    Position linePosition;
    linePosition.setVector(centroid);
    Direction lineDirection;
    lineDirection.setVector(r);
    line.setLine(linePosition, lineDirection);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(vtv/(inputPoints.size()-2.0)));
    line.setStatistic(this->statistic);

    return true;

}
