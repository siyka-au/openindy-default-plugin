#include "p_nominalpoint.h"

/*!
 * \brief NominalPoint::init
 */
void NominalPoint::init(){

    //set plugin meta data
    this->metaData.name = "NominalPoint";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "OpenIndy";
    this->metaData.description = QString("%1")
            .arg("Defines a point from hand-entered x/y/z coordinates. Produces a nominal: user-editable, no measurement involved.");
    this->metaData.iid = ConstructFunction_iidd;

    //a nominal function takes scalar parameters only - no needed elements.
    //This absence is exactly what Geometry::getOwnerDescription reads to
    //know this result is not owned by a fit.
    this->neededElements.clear();

    //set applicable for
    this->applicableFor.append(ePointFeature);

    //scalar input parameters, in the job's default unit (metres)
    this->doubleParameters.insert("x [m]", 0.0);
    this->doubleParameters.insert("y [m]", 0.0);
    this->doubleParameters.insert("z [m]", 0.0);

}

/*!
 * \brief NominalPoint::exec
 * \param point
 * \return
 */
bool NominalPoint::exec(Point &point){

    //user-entered values, falling back to the declared defaults - same
    //pattern as ObjectTransformation::ChangeRadius
    double x = this->doubleParameters.value("x [m]", 0.0);
    double y = this->doubleParameters.value("y [m]", 0.0);
    double z = this->doubleParameters.value("z [m]", 0.0);
    if(this->scalarInputParams.doubleParameter.contains("x [m]")){
        x = this->scalarInputParams.doubleParameter.value("x [m]");
    }
    if(this->scalarInputParams.doubleParameter.contains("y [m]")){
        y = this->scalarInputParams.doubleParameter.value("y [m]");
    }
    if(this->scalarInputParams.doubleParameter.contains("z [m]")){
        z = this->scalarInputParams.doubleParameter.value("z [m]");
    }

    point.setPoint(Position(x, y, z));

    //no uncertainty - this is a specification, not a measurement
    //(tolerance is deferred and has nowhere to attach yet)
    return true;

}
