#include "p_bestfitsphere.h"

/*!
 * \brief BestFitSphere::init
 */
void BestFitSphere::init(){

    //set plugin meta data
    this->metaData.name = "BestFitSphere";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted sphere.")
            .arg("You can input as many points as you want which are then used to find the best fit sphere.");
    this->metaData.iid = FitFunction_iidd;

    //D12/Stage 7c-ii: consolidates BestFitSphere (raw Observations) and
    //SphereFromPoints (Points, not tag-resolved, and missing the
    //iterative refinement step below - see the fit() call) into one
    //tag-resolved, Point-consuming declaration.
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least four points to calculate the best fit sphere.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    param1.roleName = "default";
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eSphereFeature);

}

/*!
 * \brief BestFitSphere::exec
 * \param sphere
 * \return
 */
bool BestFitSphere::exec(Sphere &sphere){

    this->statistic.reset();

    if(!this->inputElements.contains(0) || this->inputElements[0].isEmpty()){
        emit this->sendMessage(QString("No points tagged for sphere %1").arg(sphere.getFeatureName()), eWarningMessage);
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
    if(inputPoints.size() < 4){
        emit this->sendMessage(QString("Not enough valid points to fit the sphere %1").arg(sphere.getFeatureName()), eWarningMessage);
        return false;
    }

    //fit the sphere
    if(this->approximate(sphere, inputPoints)){
        return this->fit(sphere, inputPoints, allUsablePoints);
    }
    return false;

}

/*!
 * \brief BestFitSphere::approximate
 * \param sphere
 * \param inputPoints
 * \return
 */
bool BestFitSphere::approximate(Sphere &sphere, const QList<QPointer<Point> > &inputPoints){

    //calculate centroid coordinates
    OiVec centroid(3);
    for(int i = 0; i < inputPoints.size(); i++){
        centroid = centroid + inputPoints.at(i)->getPosition().getVector();
    }
    centroid = centroid / inputPoints.size();

    //Drixler
    OiMat N(4, 4);
    OiVec n(4);
    for(int i = 0; i < inputPoints.size(); i++){
        const OiVec pos = inputPoints.at(i)->getPosition().getVector();

        double x = pos.getAt(0) - centroid.getAt(0);
        double y = pos.getAt(1) - centroid.getAt(1);
        double z = pos.getAt(2) - centroid.getAt(2);

        double xx = x*x;
        double yy = y*y;
        double zz = z*z;
        double xy = x*y;
        double xz = x*z;
        double yz = y*z;
        double xyz2 = x*x + y*y + z*z;

        N.setAt(0,0, N.getAt(0,0) + xx);
        N.setAt(0,1, N.getAt(0,1) + xy);
        N.setAt(0,2, N.getAt(0,2) + xz);
        N.setAt(0,3, N.getAt(0,3) + x);
        N.setAt(1,0, N.getAt(1,0) + xy);
        N.setAt(1,1, N.getAt(1,1) + yy);
        N.setAt(1,2, N.getAt(1,2) + yz);
        N.setAt(1,3, N.getAt(1,3) + y);
        N.setAt(2,0, N.getAt(2,0) + xz);
        N.setAt(2,1, N.getAt(2,1) + yz);
        N.setAt(2,2, N.getAt(2,2) + zz);
        N.setAt(2,3, N.getAt(2,3) + z);
        N.setAt(3,0, N.getAt(3,0) + x);
        N.setAt(3,1, N.getAt(3,1) + y);
        N.setAt(3,2, N.getAt(3,2) + z);
        N.setAt(3,3, N.getAt(3,3) + 1.0);

        n.setAt(0, n.getAt(0) + x*xyz2);
        n.setAt(1, n.getAt(1) + y*xyz2);
        n.setAt(2, n.getAt(2) + z*xyz2);
        n.setAt(3, n.getAt(3) + xyz2);
    }

    OiMat Q(4,4);
    try{
        Q = N.inv();
    }catch(exception &e){
        emit this->sendMessage(e.what(), eErrorMessage);
        return false;
    }

    OiVec a = -1.0 * Q * n;

    double r = 0.0;
    double xm = 0.0;
    double ym = 0.0;
    double zm = 0.0;

    r = qSqrt( qAbs( 0.25 * (a.getAt(0)*a.getAt(0) + a.getAt(1)*a.getAt(1) + a.getAt(2)*a.getAt(2)) - a.getAt(3) ) );
    xm = -0.5 * a.getAt(0) + centroid.getAt(0);
    ym = -0.5 * a.getAt(1) + centroid.getAt(1);
    zm = -0.5 * a.getAt(2) + centroid.getAt(2);

    //set approximate result
    Position position;
    position.setVector(xm, ym, zm);
    Radius radius;
    radius.setRadius(r);
    sphere.setSphere(position, radius);

    return true;

}

/*!
 * \brief BestFitSphere::fit
 * \param sphere
 * \param inputPoints
 * \return
 */
bool BestFitSphere::fit(Sphere &sphere, const QList<QPointer<Point> > &inputPoints, const QList<QPointer<Point> > &allUsablePoints){

    int numIterations = 0;

    //set approximation values
    double xm = sphere.getPosition().getVector().getAt(0);
    double ym = sphere.getPosition().getVector().getAt(1);
    double zm = sphere.getPosition().getVector().getAt(2);
    double r = sphere.getRadius().getRadius();

    OiVec xd(4);
    double xdxd = 0.0;
    OiVec verb(inputPoints.size()*3);
    do{

        OiMat A(inputPoints.size(), 4);
        OiMat B(inputPoints.size(), inputPoints.size()*3);
        OiVec w(inputPoints.size());
        for(int i = 0; i < inputPoints.size(); i++){

            const OiVec x = inputPoints.at(i)->getPosition().getVector();

            double r0 = qSqrt( (x.getAt(0) + verb.getAt(i*3) - xm) * (x.getAt(0) + verb.getAt(i*3) - xm)
                               + (x.getAt(1) + verb.getAt(i*3+1) - ym) * (x.getAt(1) + verb.getAt(i*3+1) - ym)
                               + (x.getAt(2) + verb.getAt(i*3+2) - zm) * (x.getAt(2) + verb.getAt(i*3+2) - zm) );

            A.setAt(i, 0, -1.0 * (x.getAt(0) + verb.getAt(i*3) - xm) / r0);
            A.setAt(i, 1, -1.0 * (x.getAt(1) + verb.getAt(i*3+1) - ym) / r0);
            A.setAt(i, 2, -1.0 * (x.getAt(2) + verb.getAt(i*3+2) - zm) / r0);
            A.setAt(i, 3, -1.0);

            B.setAt(i, i*3, (x.getAt(0) + verb.getAt(i*3) - xm) / r0);
            B.setAt(i, i*3+1, (x.getAt(1) + verb.getAt(i*3+1) - ym) / r0);
            B.setAt(i, i*3+2, (x.getAt(2) + verb.getAt(i*3+2) - zm) / r0);

            w.setAt(i, r0 - r);

        }

        OiMat BBT = B * B.t();
        OiMat AT = A.t();

        OiMat N(inputPoints.size()+4, inputPoints.size()+4);
        for(int i = 0; i < BBT.getRowCount(); i++){
            for(int j = 0; j < BBT.getColCount(); j++){
                N.setAt(i,j, BBT.getAt(i,j));
            }
        }
        for(int i = 0; i < inputPoints.size(); i++){
            for(int j = 0; j < 4; j++){
                N.setAt(i, j+inputPoints.size(), A.getAt(i,j));
            }
        }
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < inputPoints.size(); j++){
                N.setAt(i+inputPoints.size(), j, AT.getAt(i,j));
            }
        }

        for(int i = 0; i < 4; i++){
            w.add(0.0);
        }

        OiMat Q(inputPoints.size()+4, inputPoints.size()+4);
        try{
            Q = N.inv();
        }catch(exception &e){
            emit this->sendMessage(e.what(), eErrorMessage);
            return false;
        }

        OiVec res = -1.0 * Q * w;

        OiVec k(inputPoints.size());
        for(int i = 0; i < inputPoints.size(); i++){
            k.setAt(i, res.getAt(i));
        }
        for(int i = 0; i < 4; i++){
            xd.setAt(i, res.getAt(i+inputPoints.size()));
        }

        OiVec vd = B.t() * k;

        //Unbekannte und Beobachtungen verbessern
        verb = verb + vd;
        xm += xd.getAt(0);
        ym += xd.getAt(1);
        zm += xd.getAt(2);
        r += xd.getAt(3);

        numIterations++;

        OiVec::dot(xdxd, xd,xd);

    }while( (xdxd > 0.000001) && (numIterations < 101) );

    if(numIterations >= 101){
        emit this->sendMessage("No solution found during 100 iterations", eWarningMessage);
        return false;
    }

    //set final result
    Position position;
    position.setVector(xm, ym, zm);
    Radius radius;
    radius.setRadius(r);
    sphere.setSphere(position, radius);

    //calculate display residuals for each observation
    for(int i = 0; i < allUsablePoints.size(); i++){
        OiVec xyz = allUsablePoints[i]->getPosition().getVector();
        OiVec dr = xyz - position.getVector();
        this->addDisplayResidual(allUsablePoints[i]->getId(), dr.length() - r);
    }

    //calculate standard deviation
    double stdv = 0.0;
    if(verb.getSize() > 12){
        double sum_vv = 0.0;
        for(int i = 0; i < verb.getSize(); i++){
            sum_vv += verb.getAt(i) * verb.getAt(i);
        }
        stdv = qSqrt(sum_vv / (verb.getSize() - 12));
    }else{
        stdv = 0.0;
    }

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(stdv);
    sphere.setStatistic(this->statistic);

    return true;

}
