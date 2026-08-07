#ifndef P_HELMERT6PARAM_H
#define P_HELMERT6PARAM_H

#include "p_helmert7Param.h"

using namespace oi;

/*!
 * \brief The Helmert6Param class is a helmert transformation without scale
 * (scale fixed to 1). Reuses Helmert7Param's math by restricting its
 * "calculate scale" parameter to "no", which routes Helmert7Param::exec()
 * to its existing unscaled 6-parameter (calc_6p) path.
 */
class Helmert6Param : public Helmert7Param
{
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

};

#endif // P_HELMERT6PARAM_H
