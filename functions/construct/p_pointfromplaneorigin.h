#ifndef P_POINTFROMPLANEORIGIN_H
#define P_POINTFROMPLANEORIGIN_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"

using namespace oi;

/*!
 * \brief The PointFromPlaneOrigin class
 * Creates a point at a plane's own position - the centroid a best-fit plane
 * solved to.
 *
 * The counterpart to LineFromPlaneNormal, and the second half of "a fit
 * gives you a plane, and the plane's normal and origin are useful features
 * in their own right". Same reasoning as D8: a declared function on a
 * user-created feature, not a function emitting features on its own.
 *
 * Note this is a derived feature, not a measured one - it inherits whatever
 * the plane's fit produced. It is emphatically not a Point in the sense
 * that D9 means (something an instrument actually observed); nothing was
 * measured here.
 */
class PointFromPlaneOrigin : public ConstructFunction
{
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

protected:

    //############
    //exec methods
    //############

    bool exec(Point &point);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);

};

#endif // P_POINTFROMPLANEORIGIN_H
