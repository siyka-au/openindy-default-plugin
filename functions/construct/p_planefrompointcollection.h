#ifndef P_PLANEFROMPOINTCOLLECTION_H
#define P_PLANEFROMPOINTCOLLECTION_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "constructfunction.h"
#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The PlaneFromPointCollection class
 * Fits a plane to a whole PointCollection bound as a single input, rather
 * than to N individually bound points.
 *
 * The difference is not cosmetic. Binding points one at a time means the
 * function's own input list grows with every point acquired, so automating
 * a thousand reflectorless shots across a factory wall would mean a
 * thousand input elements to manage. Binding one collection means bulk
 * acquisition just adds points to the collection and this function is never
 * touched - it re-solves because the collection changed, through the
 * dependency graph that already exists.
 *
 * It also stops caring where the points came from: a PointGroup of
 * individually aimed shots and a scanner's PointCloud both satisfy
 * PointCollection, and fitting a plane is the same operation either way.
 */
class PlaneFromPointCollection : public ConstructFunction, public BestFitPlaneUtil
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

    bool exec(Plane &plane);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Plane &plane);

};

#endif // P_PLANEFROMPOINTCOLLECTION_H
