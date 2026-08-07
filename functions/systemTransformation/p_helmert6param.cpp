#include "p_helmert6param.h"

/*!
 * \brief Helmert6Param::init
 */
void Helmert6Param::init(){

    Helmert7Param::init();

    //set plugin meta data
    this->metaData.name = "Helmert6Param";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra, jwa";
    this->metaData.description = QString("%1")
            .arg("This function calculates a helmert transformation without scale (scale fixed to 1).");
    this->metaData.iid = SystemTransformation_iidd;

    //restrict to the no-scale path - Helmert7Param::exec() already calls calc_6p() when this resolves to "no"
    this->stringParameters.remove("calculate scale");
    this->stringParameters.insert("calculate scale", "no");

    //drop scale-only parameters that no longer apply
    this->stringParameters.remove("use temperature");
    this->stringParameters.remove("use reference temperature");
    this->stringParameters.remove("material");
    this->doubleParameters.remove("reference");
    this->doubleParameters.remove("actual");

}
